#include <QtWidgets>

#include "global.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent) {
    mainwindow = this;

    setupCodeViewer();
    setupGraphViewer();

    QHBoxLayout *hBox = new QHBoxLayout;
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spLeft.setHorizontalStretch(4);
    spRight.setHorizontalStretch(3);
    editor->setSizePolicy(spLeft);
    view->setSizePolicy(spRight);
    splitter->addWidget(editor);
    splitter->addWidget(view);
    hBox->addWidget(splitter);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(hBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Adjoint Code Debugger"));
}

void MainWindow::openFile(const QString &path) {
    fileName = path;

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            editor->setPlainText(file.readAll());
            QString dotFile("small_dag.dot");
            g_dag_layouter.parseDotFile(dotFile);
        }
    }

    offsetinfo.get_mapping(fileName.toUtf8().constData());
}

void MainWindow::init(long nodeidx) {
    if (isNotSetParam) {
        setParam();
        firstNodeId = nodeidx;
        isNotSetParam = false;
    }

    g_dag.read_huge_dot_file();
    g_dag_extractor._dag = g_dag;
    g_dag_extractor.set_current_node(nodeidx);
    g_dag_extractor.write_dot_file();

    int statementidx = (*g_nodeidx2assignmentidxmap)[nodeidx];
    currIndex = statementidx;
    const mapping_helper::position_info &pos = g_mapping_helper.location(statementidx);

    QString filename(pos.filename.c_str());
    this->openFile(filename);
    this->setHighlightCode(currIndex);
    connect(this, &MainWindow::callCreateGraph, scene, &GraphicsScene::createGraph);
    this->send1();
    connect(this, &MainWindow::callSetHighlightScene1, scene, &GraphicsScene::setHighlightScene1);
    this->send2(nodeidx);

    std::cout << "Layer Number: " << g_dag_extractor._maxhop << std::endl;
    std::cout << "Time To Live: " << g_dag_extractor.ttl << std::endl;
    std::cout << "Node ID: " << nodeidx << std::endl;
}

void MainWindow::setParam() {
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Set Parameters"));

    QFormLayout form(&dialog);
    QList<QLineEdit *> fields;
    QLineEdit *lineEdit1 = new QLineEdit(&dialog);
    QString label1 = QString("Layer Number:");

    QLineEdit *lineEdit2 = new QLineEdit(&dialog);
    QString label2 = QString("Time To Live:");

    lineEdit1->setPlaceholderText("1");
    form.addRow(label1, lineEdit1);
    fields << lineEdit1;

    lineEdit2->setPlaceholderText("1");
    form.addRow(label2, lineEdit2);
    fields << lineEdit2;

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted) {
        std::string l = fields.at(0)->text().toUtf8().constData();
        std::string t = fields.at(1)->text().toUtf8().constData();

        if (l != "")
            g_dag_extractor._maxhop = std::atoi(l.c_str());

        if (t != "")
            g_dag_extractor.ttl = std::atoi(t.c_str());
    }
}

void MainWindow::setHighlightCode(unsigned id) {
    int line = g_mapping_helper.location(id).line;
    int col = g_mapping_helper.location(id).col;
    int start = offsetinfo.get_position(line, col);
    int end = -1;

    if (id > 0 && g_mapping_helper._positions[id-1].filename == fileName.toUtf8().constData()) {
        if (id < g_mapping_helper._positions.size()) {
            if (g_mapping_helper._positions[id].filename == fileName.toUtf8().constData() && g_mapping_helper._positions[id].line == line) {
                end = offsetinfo.get_position(g_mapping_helper._positions[id].line, g_mapping_helper._positions[id].col);
            }

            else {
                int diff = offsetinfo.get_position(line+1, 1) - offsetinfo.get_position(line, 1);
                end = offsetinfo.get_position(line, 1) + diff - 1;
            }
        }

        else {
            int diff = offsetinfo.get_position(line+1, 1) - offsetinfo.get_position(line, 1);
            end = offsetinfo.get_position(line, 1) + diff - 1;
        }

        lastStart = start;
        QTextCursor cursor = editor->textCursor();
        cursor.setPosition(start);
        cursor.setPosition(end, QTextCursor::KeepAnchor);
        editor->setTextCursor(cursor);
    }

    else {
        QTextCursor cursor = editor->textCursor();
        cursor.setPosition(lastStart);
        cursor.setPosition(lastStart, QTextCursor::KeepAnchor);
        editor->setTextCursor(cursor);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
   view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::send1() const {
    emit callCreateGraph();
}

void MainWindow::send2(long id) const {
    emit callSetHighlightScene1(id);
}

void MainWindow::setupCodeViewer() {
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new CodeViewer(this);
    editor->setFont(font);
    editor->setReadOnly(true);

    highlighter = new Highlighter(editor->document());
}

void MainWindow::setupGraphViewer() {
    view = new QGraphicsView();
    scene = new GraphicsScene();
    view->setScene(scene);
}

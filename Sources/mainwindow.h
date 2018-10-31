#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "highlighter.h"
#include "linenumber.h"
#include "file_line_number_index.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void init(long nodeidx);
    void setHighlightCode(unsigned id);

signals:
    void callCreateGraph() const;
    void callSetHighlightScene1(long id) const;

public slots:
    void openFile(const QString &path = QString());
    void setParam();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void setupCodeViewer();
    void setupGraphViewer();
    void send1() const;
    void send2(long id) const;

    Highlighter *highlighter;
    QString fileName;
    file_line_number_index offsetinfo;
};

#endif // MAINWINDOW_H

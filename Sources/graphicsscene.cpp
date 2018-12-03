#include "global.h"
#include "graphicsscene.h"
#include "graphics_view_zoom.h"

GraphicsScene::GraphicsScene(QGraphicsScene *parent):
    QGraphicsScene(parent) {
    itemTextPair = new QMap<QGraphicsItem*, GraphicsTextItem*>();
    itemNodePair = new QMap<QGraphicsItem*, Node>();
    itemMap = new QMap<QGraphicsItem*, QMap<GraphicsTextItem*, GraphicsPathItem*>*>();

    Graphics_view_zoom *zoom = new Graphics_view_zoom(view);
    zoom->set_modifiers(Qt::NoModifier);

    view->setRenderHint(QPainter::Antialiasing);
}

void GraphicsScene::createGraph() const {
    QList<Node>::iterator node;
    for (node = g_dag_layouter.nodes.begin(); node != g_dag_layouter.nodes.end(); node++) {
        QRect *EBounding = new QRect((*node).pos_x - (*node).width/2, (*node).pos_y - (*node).height/2,
                                     (*node).width, (*node).height);
        GraphicsRectItem *rectItem = new GraphicsRectItem(*EBounding);
        QPen pen;
        pen.setColor(Qt::green);
        rectItem->setPen(pen);
        rects.push_back(rectItem);

        GraphicsTextItem *nodel = new GraphicsTextItem();
        std::string label = (*node).label.toUtf8().constData();
        long nodeidx = std::atol(label.c_str());
        std::stringstream tmpbuf;
        if (nodeidx <= firstNodeId)
            tmpbuf << nodeidx << "\n" << std::scientific << g_adjoint_vector[nodeidx];
        else
            tmpbuf << nodeidx << "\n INVALID INDEX!!!";
        nodel->setPlainText(QString::fromStdString(tmpbuf.str()));
        nodel->setTextWidth((*node).width);
        QTextBlockFormat format;
        format.setAlignment(Qt::AlignCenter);
        QTextCursor cursor = nodel->textCursor();
        cursor.select(QTextCursor::Document);
        cursor.mergeBlockFormat(format);
        cursor.clearSelection();
        nodel->setTextCursor(cursor);
        double nodel_boundingW = nodel->boundingRect().width();
        double nodel_boundingH = nodel->boundingRect().height();
        nodel->setPos((*node).pos_x-nodel_boundingW/2, (*node).pos_y-nodel_boundingH/2);
        rlabels.push_back(nodel);

        itemNodePair->insert(rectItem, (*node));
        itemTextPair->insert(rectItem, nodel);
        QMap<GraphicsTextItem*, GraphicsPathItem*> *temp = new QMap<GraphicsTextItem*, GraphicsPathItem*>();
        itemMap->insert(rectItem, temp);
    }

    QList<Edge>::iterator edge;
    for (edge = g_dag_layouter.edges.begin(); edge != g_dag_layouter.edges.end(); edge++) {
        GraphicsPathItem *path = new GraphicsPathItem((*edge).edgePath);
        paths.push_back(path);

        GraphicsPathItem *arrow = new GraphicsPathItem((*edge).arrow);
        arrow->setBrush(Qt::black);
        arrows.push_back(arrow);

        GraphicsTextItem *edgel = new GraphicsTextItem();
        edgel->setPlainText((*edge).label);
        double edgel_boundingW = edgel->boundingRect().width();
        double edgel_boundingH = edgel->boundingRect().height();
        edgel->setPos((*edge).lp_x-edgel_boundingW/2, (*edge).lp_y-edgel_boundingH/2);
        plabels.push_back(edgel);

        for(auto rect: itemNodePair->keys()){
            QList<Edge> le = itemNodePair->value(rect).linkedE;

            QList<Edge>::iterator e;
            for (e = le.begin(); e != le.end(); e++){
                if ((*e) == (*edge)){
                    itemMap->value(rect)->insertMulti(edgel, path);
                    itemMap->value(rect)->insertMulti(edgel, arrow);
                }
            }
        }
    }

    for (auto const& path: paths) {
        scene->addItem(path);
    }

    for (auto const& plabel: plabels) {
        scene->addItem(plabel);
    }

    for (auto const& arrow: arrows) {
        scene->addItem(arrow);
    }

    for (auto const& rlabel: rlabels) {
        scene->addItem(rlabel);
    }

    for (auto const& rect: rects) {
        scene->addItem(rect);
    }
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        preP = event->scenePos();

        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        GraphicsRectItem *selectedRect = qgraphicsitem_cast<GraphicsRectItem *>(item);

        QList<QGraphicsItem*> items = scene->items();
        QList<QGraphicsItem*>::iterator each;
        for (each = items.begin(); each != items.end(); each++) {
            GraphicsRectItem *e = qgraphicsitem_cast<GraphicsRectItem *>((*each));
            GraphicsPathItem *l = qgraphicsitem_cast<GraphicsPathItem *>((*each));
            GraphicsTextItem *t = qgraphicsitem_cast<GraphicsTextItem *>((*each));

            if (e)
                e->setPen(QPen(Qt::green));
            else if (l) {
                l->setPen(QPen(Qt::black));
                for(auto arrow: arrows) {
                    if (arrow == l)
                        l->setBrush(Qt::black);
                }
            }
            else
                t->setDefaultTextColor(Qt::black);

            (*each)->setFlag(QGraphicsItem::ItemIsMovable, true);
            (*each)->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }

        if(selectedRect) {
            selectedRect->setPen(QPen(Qt::red));
            itemTextPair->value(selectedRect)->setDefaultTextColor(Qt::red);

            for(auto edgel: itemMap->value(selectedRect)->keys()) {
                edgel->setDefaultTextColor(Qt::red);
                itemMap->value(selectedRect)->values(edgel)[0]->setBrush(Qt::red);
                itemMap->value(selectedRect)->values(edgel)[0]->setPen(QPen(Qt::red));
                itemMap->value(selectedRect)->values(edgel)[1]->setPen(QPen(Qt::red));
            }

            Node node = itemNodePair->value(selectedRect);
            std::string label = node.label.toUtf8().constData();
            int nodeidx = std::atoi(label.c_str());
            currIndex = (*g_nodeidx2assignmentidxmap)[nodeidx];
            connect(scene, &GraphicsScene::callSetHighlightCode, mainwindow, &MainWindow::setHighlightCode);
            this->send(currIndex);
        }
        else {
            connect(scene, &GraphicsScene::callSetHighlightCode, mainwindow, &MainWindow::setHighlightCode);
            this->send(0);
        }
    }

    QGraphicsScene::mousePressEvent(event);

    if (event->button() == Qt::RightButton) {
        QString fileName = QFileDialog::getSaveFileName(view, "Export PDF", QString(), "*.pdf");
        if (QFileInfo(fileName).suffix().isEmpty()) {fileName.append(".pdf");}

        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(fileName);

        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter);
    }
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    int distance;

    if (event->buttons() & Qt::LeftButton) {
        distance = (event->scenePos() - preP).manhattanLength();
        view->setCursor(Qt::ClosedHandCursor);

        QList<QGraphicsItem*> items = scene->items();
        QList<QGraphicsItem*>::iterator each;
        for (each = items.begin(); each != items.end(); each++)
            (*each)->setSelected(true);
    }

    if (distance > QApplication::startDragDistance()) {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    view->setCursor(Qt::ArrowCursor);

    QList<QGraphicsItem*> items = scene->items();
    QList<QGraphicsItem*>::iterator each;
    for (each = items.begin(); each != items.end(); each++) {
        (*each)->setFlag(QGraphicsItem::ItemIsMovable, false);
        (*each)->setFlag(QGraphicsItem::ItemIsSelectable, false);
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsScene::setHighlightScene1(long id) const {
    for(auto item : itemNodePair->keys()) {
        Node node = itemNodePair->value(item);
        std::string label = node.label.toUtf8().constData();
        int nodeidx = std::atoi(label.c_str());

        if (nodeidx != id)
            continue;

        GraphicsRectItem *selectedRect = qgraphicsitem_cast<GraphicsRectItem *>(item);

        if(selectedRect){
            selectedRect->setPen(QPen(Qt::red));
            itemTextPair->value(selectedRect)->setDefaultTextColor(Qt::red);

            for(auto edgel: itemMap->value(selectedRect)->keys()){
                edgel->setDefaultTextColor(Qt::red);
                itemMap->value(selectedRect)->values(edgel)[0]->setBrush(Qt::red);
                itemMap->value(selectedRect)->values(edgel)[0]->setPen(QPen(Qt::red));
                itemMap->value(selectedRect)->values(edgel)[1]->setPen(QPen(Qt::red));
            }
        }
    }
}

void GraphicsScene::setHighlightScene2(long id) const {
    QList<QGraphicsItem*> items = scene->items();
    QList<QGraphicsItem*>::iterator each;
    for (each = items.begin(); each != items.end(); each++) {
        GraphicsRectItem *e = qgraphicsitem_cast<GraphicsRectItem *>((*each));
        GraphicsPathItem *l = qgraphicsitem_cast<GraphicsPathItem *>((*each));
        GraphicsTextItem *t = qgraphicsitem_cast<GraphicsTextItem *>((*each));

        if (e)
            e->setPen(QPen(Qt::green));
        else if (l) {
            l->setPen(QPen(Qt::black));
            for(auto arrow: arrows){
                if (arrow == l)
                    l->setBrush(Qt::black);
            }
        }
        else
            t->setDefaultTextColor(Qt::black);

        (*each)->setFlag(QGraphicsItem::ItemIsMovable, true);
        (*each)->setFlag(QGraphicsItem::ItemIsSelectable, true);
    }

    if (id > 0) {
        for(auto item : itemNodePair->keys()) {
            Node node = itemNodePair->value(item);
            std::string label = node.label.toUtf8().constData();
            int nodeidx = std::atoi(label.c_str());

            if ((*g_nodeidx2assignmentidxmap)[nodeidx] != id)
                continue;

            GraphicsRectItem *selectedRect = qgraphicsitem_cast<GraphicsRectItem *>(item);

            if(selectedRect){
                selectedRect->setPen(QPen(Qt::red));
                itemTextPair->value(selectedRect)->setDefaultTextColor(Qt::red);

                for(auto edgel: itemMap->value(selectedRect)->keys()){
                    edgel->setDefaultTextColor(Qt::red);
                    itemMap->value(selectedRect)->values(edgel)[0]->setBrush(Qt::red);
                    itemMap->value(selectedRect)->values(edgel)[0]->setPen(QPen(Qt::red));
                    itemMap->value(selectedRect)->values(edgel)[1]->setPen(QPen(Qt::red));
                }
            }
        }
    }
}

void GraphicsScene::send(int id) const {
    emit callSetHighlightCode(id);
}

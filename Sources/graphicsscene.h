#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include "graph.h"
#include "graphicsrectitem.h"
#include "graphicstextitem.h"
#include "graphicspathitem.h"

#include <QGraphicsScene>
#include <QPrinter>

class GraphicsScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GraphicsScene(QGraphicsScene *parent = 0);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void callSetHighlightCode(long id) const;

public slots:
    void createGraph() const;
    void setHighlightScene1(long id) const;
    void setHighlightScene2(long id) const;

private:
    void send(int id) const;

    QPointF preP;
    mutable QVector<GraphicsRectItem*> rects;
    mutable QVector<GraphicsTextItem*> rlabels;
    mutable QVector<GraphicsPathItem*> paths;
    mutable QVector<GraphicsPathItem*> arrows;
    mutable QVector<GraphicsTextItem*> plabels;
    QMap<QGraphicsItem*, GraphicsTextItem*> *itemTextPair;
    QMap<QGraphicsItem*, Node> *itemNodePair;
    QMap<QGraphicsItem*, QMap<GraphicsTextItem*, GraphicsPathItem*>*> *itemMap;
};

#endif // GRAPHICSSCENE_H

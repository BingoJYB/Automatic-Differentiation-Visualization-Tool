#ifndef GRAPHICSLINEITEM_H
#define GRAPHICSLINEITEM_H

#include <QGraphicsPathItem>

class GraphicsPathItem : public QGraphicsPathItem {
public :
    GraphicsPathItem(QPainterPath & path, QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // GRAPHICSLINEITEM_H

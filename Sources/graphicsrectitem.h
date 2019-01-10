#ifndef GRAPHICSELLIPSEITEM_H
#define GRAPHICSELLIPSEITEM_H

#include <QGraphicsRectItem>

class GraphicsRectItem : public QGraphicsRectItem {
public :
    GraphicsRectItem(const QRectF &rect, QGraphicsItem *parent = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // GRAPHICSELLIPSEITEM_H

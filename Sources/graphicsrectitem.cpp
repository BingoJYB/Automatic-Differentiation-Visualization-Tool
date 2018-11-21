#include "graphicsrectitem.h"

#include <QStyle>
#include <QStyleOptionGraphicsItem>

GraphicsRectItem::GraphicsRectItem(const QRectF &rect, QGraphicsItem *parent) :
        QGraphicsRectItem(rect, parent) {

}

void GraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QStyleOptionGraphicsItem newOption(*option);
    newOption.state = QStyle::State_None;
    QGraphicsRectItem::paint(painter, &newOption, widget);
}

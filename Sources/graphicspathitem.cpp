#include "graphicspathitem.h"

#include <QStyle>
#include <QStyleOptionGraphicsItem>

GraphicsPathItem::GraphicsPathItem(QPainterPath &path, QGraphicsItem *parent) :
        QGraphicsPathItem(path, parent) {

}

void GraphicsPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QStyleOptionGraphicsItem newOption(*option);
    newOption.state = QStyle::State_None;
    QGraphicsPathItem::paint(painter, &newOption, widget);
}

#include "graphicstextitem.h"

#include <QStyle>
#include <QStyleOptionGraphicsItem>

GraphicsTextItem::GraphicsTextItem(QGraphicsItem *parent) :
        QGraphicsTextItem(parent) {

}

void GraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QStyleOptionGraphicsItem newOption(*option);
    newOption.state = QStyle::State_None;
    QGraphicsTextItem::paint(painter, &newOption, widget);
}

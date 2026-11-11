#include "paragraphitem.h"

#include <QPainter>
#include <QPen>

ParagraphItem::ParagraphItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

QRectF ParagraphItem::boundingRect() const
{
    // a little extra so the outline never gets clipped
    return QRectF(-boxSize.width() / 2, -boxSize.height() / 2,
                  boxSize.width(), boxSize.height()).adjusted(-2, -2, 2, 2);
}

void ParagraphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF r(-boxSize.width() / 2, -boxSize.height() / 2, boxSize.width(), boxSize.height());
    painter->fillRect(r, color);

    // outline is only for editing, cosmetic keeps it 1px at any zoom
    QPen pen(QColor(90, 140, 255), 1, Qt::DashLine);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(r);
}

void ParagraphItem::setCorners(const QPointF &a, const QPointF &b)
{
    prepareGeometryChange();
    boxSize = QSizeF(qAbs(a.x() - b.x()), qAbs(a.y() - b.y()));
    setPos((a + b) / 2);
}

QSizeF ParagraphItem::size() const
{
    return boxSize;
}

QColor ParagraphItem::boxColor() const
{
    return color;
}

void ParagraphItem::setBoxColor(const QColor &newColor)
{
    color = newColor;
    update();
}

int ParagraphItem::textSize() const
{
    return defaultTextSize;
}

void ParagraphItem::setTextSize(int pixels)
{
    defaultTextSize = pixels;
}
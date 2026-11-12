#include "paragraphitem.h"
#include "theme.h"

#include <QLineF>
#include <QPainter>
#include <QPen>

// handle sizes are in screen pixels, pixelScale turns them into scene units
static const double handleHalf = 5;        // drawn size
static const double handleGrab = 7;        // a bit bigger so they are easy to hit
static const double rotateDistance = 28;   // knob distance from the top edge
static const double rotateRadius = 6;

static const ParagraphItem::Handle sizers[] = {
    ParagraphItem::TopLeft, ParagraphItem::Top, ParagraphItem::TopRight, ParagraphItem::Right,
    ParagraphItem::BottomRight, ParagraphItem::Bottom, ParagraphItem::BottomLeft, ParagraphItem::Left
};

ParagraphItem::ParagraphItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

QRectF ParagraphItem::box() const
{
    return QRectF(-boxSize.width() / 2, -boxSize.height() / 2, boxSize.width(), boxSize.height());
}

QRectF ParagraphItem::boundingRect() const
{
    // selected boxes need room for the handles and the rotate knob
    double margin = active ? 40 * pixelScale : 2;
    return box().adjusted(-margin, -margin, margin, margin);
}

// only the box itself counts as a hit, the handles are checked by handleAt
QPainterPath ParagraphItem::shape() const
{
    QPainterPath path;
    path.addRect(box());
    return path;
}

void ParagraphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF r = box();
    painter->fillRect(r, color);

    // outline is only for editing, cosmetic keeps it 1px at any zoom
    QPen pen(Theme::accent(), 1, Qt::DashLine);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(r);

    if (!active)
        return;

    QPen solid(Theme::accent(), 1);
    solid.setCosmetic(true);
    painter->setPen(solid);
    painter->drawLine(handlePos(Top), handlePos(Rotate));

    painter->setBrush(Qt::white);
    double h = handleHalf * pixelScale;
    for (Handle handle : sizers) {
        QPointF c = handlePos(handle);
        painter->drawRect(QRectF(c.x() - h, c.y() - h, h * 2, h * 2));
    }
    painter->drawEllipse(handlePos(Rotate), rotateRadius * pixelScale, rotateRadius * pixelScale);
}

QPointF ParagraphItem::handlePos(Handle handle) const
{
    double w = boxSize.width() / 2;
    double h = boxSize.height() / 2;

    switch (handle) {
    case TopLeft:     return QPointF(-w, -h);
    case Top:         return QPointF(0, -h);
    case TopRight:    return QPointF(w, -h);
    case Right:       return QPointF(w, 0);
    case BottomRight: return QPointF(w, h);
    case Bottom:      return QPointF(0, h);
    case BottomLeft:  return QPointF(-w, h);
    case Left:        return QPointF(-w, 0);
    case Rotate:      return QPointF(0, -h - rotateDistance * pixelScale);
    default:          return QPointF();
    }
}

// what is under this point (in item coordinates), handles win over the body
ParagraphItem::Handle ParagraphItem::handleAt(const QPointF &p) const
{
    if (QLineF(p, handlePos(Rotate)).length() <= (rotateRadius + 3) * pixelScale)
        return Rotate;

    for (Handle handle : sizers) {
        QPointF d = p - handlePos(handle);
        if (qAbs(d.x()) <= handleGrab * pixelScale && qAbs(d.y()) <= handleGrab * pixelScale)
            return handle;
    }

    return box().contains(p) ? Body : None;
}
// bounding rect of the rotated box, this is what snapping works with
QRectF ParagraphItem::pageRect() const
{
    return mapRectToParent(box());
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

void ParagraphItem::setSize(const QSizeF &size)
{
    prepareGeometryChange();
    boxSize = size;
    update();
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

void ParagraphItem::setActive(bool isActive)
{
    prepareGeometryChange();
    active = isActive;
    update();
}

void ParagraphItem::setPixelScale(double unitsPerPixel)
{
    prepareGeometryChange();
    pixelScale = unitsPerPixel;
    update();
}
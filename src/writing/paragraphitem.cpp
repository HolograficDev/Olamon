#include "paragraphitem.h"
#include "fontmanager.h"
#include "theme.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QLineF>
#include <QPainter>
#include <QPen>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextOption>

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
    setFlag(ItemClipsChildrenToShape);   // text can't spill out of the box

    text = new QGraphicsTextItem(this);
    text->setDefaultTextColor(Qt::black);   // text color options come in the next stage
    text->setFont(FontManager::font(defaultTextSize));
    text->document()->setDocumentMargin(6);
    text->setTextInteractionFlags(Qt::NoTextInteraction);
    text->setAcceptedMouseButtons(Qt::NoButton);   // only takes clicks while editing
    setAlignment(Qt::AlignLeft);
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

// text follows the box, top left corner and the width of the box
void ParagraphItem::layoutText()
{
    text->setTextWidth(qMax(1.0, boxSize.width()));
    text->setPos(-boxSize.width() / 2, -boxSize.height() / 2);
}

void ParagraphItem::setCorners(const QPointF &a, const QPointF &b)
{
    prepareGeometryChange();
    boxSize = QSizeF(qAbs(a.x() - b.x()), qAbs(a.y() - b.y()));
    setPos((a + b) / 2);
    layoutText();
}

QSizeF ParagraphItem::size() const
{
    return boxSize;
}

void ParagraphItem::setSize(const QSizeF &size)
{
    prepareGeometryChange();
    boxSize = size;
    layoutText();
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
    text->setFont(FontManager::font(pixels));
}

Qt::Alignment ParagraphItem::alignment() const
{
    return align;
}

void ParagraphItem::setAlignment(Qt::Alignment a)
{
    align = a;

    QTextOption option = text->document()->defaultTextOption();
    option.setAlignment(a);
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);   // long words wrap too
    text->document()->setDefaultTextOption(option);
}

QGraphicsTextItem *ParagraphItem::textItem() const
{
    return text;
}

void ParagraphItem::setEditing(bool on)
{
    editing = on;
    text->setTextInteractionFlags(on ? Qt::TextEditorInteraction : Qt::NoTextInteraction);
    text->setAcceptedMouseButtons(on ? Qt::AllButtons : Qt::NoButton);

    if (on) {
        text->setFocus(Qt::OtherFocusReason);
    } else {
        text->clearFocus();

        // don't leave a selection highlighted behind
        QTextCursor cursor = text->textCursor();
        cursor.clearSelection();
        text->setTextCursor(cursor);
    }
}

// clicking the empty part of a box that is being edited shouldn't drop the keyboard focus
void ParagraphItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (editing) {
        text->setFocus(Qt::MouseFocusReason);
        event->accept();
        return;
    }
    QGraphicsItem::mousePressEvent(event);
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
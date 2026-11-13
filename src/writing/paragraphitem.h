#ifndef PARAGRAPHITEM_H
#define PARAGRAPHITEM_H

#include <QColor>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QSizeF>

class QGraphicsTextItem;

// a box that holds text, origin is its center so rotating and scaling works around the middle
class ParagraphItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 1 };   // lets qgraphicsitem_cast tell paragraphs apart

    // what the mouse can grab on a selected box
    enum Handle { None, Body, TopLeft, Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left, Rotate };

    explicit ParagraphItem(QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // sets the box from two opposite corners, in the parent's coordinates
    void setCorners(const QPointF &a, const QPointF &b);

    QSizeF size() const;
    void setSize(const QSizeF &size);   // center stays where it is

    QColor boxColor() const;
    void setBoxColor(const QColor &color);

    int textSize() const;
    void setTextSize(int pixels);

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);

    // text tool stuff
    void setEditing(bool on);
    QGraphicsTextItem *textItem() const;

    // transform tool stuff
    void setActive(bool isActive);            // shows the handles
    void setPixelScale(double unitsPerPixel); // keeps the handles the same size on screen
    Handle handleAt(const QPointF &itemPos) const;
    QRectF pageRect() const;                  // box in page coordinates, rotation included

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QRectF box() const;
    QPointF handlePos(Handle handle) const;
    void layoutText();

    QSizeF boxSize;
    QColor color = Qt::transparent;
    int defaultTextSize = 48;

    QGraphicsTextItem *text;   // child item that does the actual typing
    Qt::Alignment align = Qt::AlignLeft;
    bool editing = false;

    bool active = false;
    double pixelScale = 1.0;
};

#endif // PARAGRAPHITEM_H
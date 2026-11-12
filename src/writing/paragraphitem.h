#ifndef PARAGRAPHITEM_H
#define PARAGRAPHITEM_H

#include <QColor>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QSizeF>

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

    // transform tool stuff
    void setActive(bool isActive);            // shows the handles
    void setPixelScale(double unitsPerPixel); // keeps the handles the same size on screen
    Handle handleAt(const QPointF &itemPos) const;
    QRectF pageRect() const;   // box in page coordinates, rotation included

private:
    QRectF box() const;
    QPointF handlePos(Handle handle) const;

    QSizeF boxSize;
    QColor color = Qt::transparent;
    int defaultTextSize = 48;

    bool active = false;
    double pixelScale = 1.0;
};

#endif // PARAGRAPHITEM_H
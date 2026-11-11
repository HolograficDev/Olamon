#ifndef PARAGRAPHITEM_H
#define PARAGRAPHITEM_H

#include <QColor>
#include <QGraphicsItem>
#include <QSizeF>

// a box that holds text, origin is its center so rotating and scaling works around the middle
class ParagraphItem : public QGraphicsItem
{
public:
    explicit ParagraphItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // sets the box from two opposite corners, in the parent's coordinates
    void setCorners(const QPointF &a, const QPointF &b);

    QSizeF size() const;

    QColor boxColor() const;
    void setBoxColor(const QColor &color);

    int textSize() const;
    void setTextSize(int pixels);

private:
    QSizeF boxSize;
    QColor color = Qt::transparent;
    int defaultTextSize = 48;
};

#endif // PARAGRAPHITEM_H
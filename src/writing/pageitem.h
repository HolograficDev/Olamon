#ifndef PAGEITEM_H
#define PAGEITEM_H

#include <QColor>
#include <QGraphicsItem>
#include <QSize>

// the page itself, everything else gets drawn on top of it
class PageItem : public QGraphicsItem
{
public:
    PageItem(const QSize &size, const QColor &background, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QSize pageSize;
    QColor background;
};

#endif // PAGEITEM_H
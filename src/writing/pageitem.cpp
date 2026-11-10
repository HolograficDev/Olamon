#include "pageitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>

PageItem::PageItem(const QSize &size, const QColor &color, QGraphicsItem *parent)
    : QGraphicsItem(parent), pageSize(size), background(color)
{
}

QRectF PageItem::boundingRect() const
{
    return QRectF(0, 0, pageSize.width(), pageSize.height());
}

void PageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // only paint the part that is actually visible
    QRectF r = option->exposedRect.intersected(boundingRect());
    if (r.isEmpty())
        return;

    // checkerboard behind transparent / see through backgrounds
    if (background.alpha() < 255) {
        const int cell = 32;
        painter->fillRect(r, QColor(255, 255, 255));

        int x0 = qFloor(r.left() / cell);
        int x1 = qCeil(r.right() / cell);
        int y0 = qFloor(r.top() / cell);
        int y1 = qCeil(r.bottom() / cell);

        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ++x) {
                if ((x + y) % 2 == 0) {
                    QRectF square(x * cell, y * cell, cell, cell);
                    painter->fillRect(square.intersected(r), QColor(204, 204, 204));
                }
            }
        }
    }

    painter->fillRect(r, background);
}
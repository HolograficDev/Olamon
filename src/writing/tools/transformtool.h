#ifndef TRANSFORMTOOL_H
#define TRANSFORMTOOL_H

#include "tool.h"
#include "paragraphitem.h"

#include <QList>
#include <QMetaObject>
#include <QPointF>
#include <QSizeF>

class QCheckBox;
class QDoubleSpinBox;
class QGraphicsLineItem;
class QSpinBox;

class TransformTool : public Tool
{
    Q_OBJECT

public:
    using Tool::Tool;

    QString name() const override;
    Qt::CursorShape cursor() const override;

    void activated() override;
    void deactivated() override;

    void mousePressed(const QPointF &scenePos) override;
    void mouseMoved(const QPointF &scenePos) override;
    void mouseReleased(const QPointF &scenePos) override;
    bool keyPressed(int key) override;

protected:
    QWidget *createOptionsWidget() override;

private:
    void select(ParagraphItem *item);
    void deleteSelected();
    void startDrag(ParagraphItem::Handle handle, const QPointF &pagePos);
    void resizeTo(QPointF pagePos);
    void onZoomChanged();
    void updateFields();   // box -> spin boxes
    void applyFields();    // spin boxes -> box

    // snapping
    bool snapOn() const;
    double snapThreshold() const;
    void collectTargets(QList<double> &xs, QList<double> &ys) const;
    void snapMove();
    void showGuides(const QList<double> &xs, const QList<double> &ys);
    void clearGuides();

    ParagraphItem *selected = nullptr;
    ParagraphItem::Handle hover = ParagraphItem::None;
    ParagraphItem::Handle dragging = ParagraphItem::None;

    // how the box looked when the drag started, in page coordinates
    QPointF pressPos;
    QPointF startCenter;
    QSizeF startSize;
    double startRotation = 0;

    bool snapping = true;
    QList<QGraphicsLineItem *> guides;   // the magenta lines, children of the page

    QMetaObject::Connection zoomConnection;
    QMetaObject::Connection resetConnection;

    QCheckBox *snapBox = nullptr;
    QSpinBox *xBox = nullptr;
    QSpinBox *yBox = nullptr;
    QSpinBox *widthBox = nullptr;
    QSpinBox *heightBox = nullptr;
    QDoubleSpinBox *rotationBox = nullptr;
};

#endif // TRANSFORMTOOL_H
#include "transformtool.h"
#include "canvasview.h"
#include "pageitem.h"
#include "theme.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGraphicsLineItem>
#include <QGuiApplication>
#include <QLabel>
#include <QPen>
#include <QSpinBox>
#include <QTransform>
#include <QVBoxLayout>
#include <QtMath>
#include <cmath>

using H = ParagraphItem;

static bool shiftHeld()
{
    return QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier);
}

static bool ctrlHeld()
{
    return QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
}

// what a snap on one axis found: how far to shift, and which lines it ended up on
struct SnapResult
{
    double shift = 0;
    QList<double> lines;
};

// looks for the closest target to any of my positions, nothing within the threshold = no shift
static SnapResult snapAxis(const QList<double> &mine, const QList<double> &targets, double threshold)
{
    SnapResult result;
    double best = threshold;
    bool found = false;

    for (double m : mine) {
        for (double t : targets) {
            double d = t - m;
            if (qAbs(d) <= best) {
                best = qAbs(d);
                result.shift = d;
                found = true;
            }
        }
    }
    if (!found)
        return result;

    // after the shift, every target we landed on gets a guide (not just the first one)
    for (double m : mine) {
        for (double t : targets) {
            if (qAbs(t - (m + result.shift)) < 0.5 && !result.lines.contains(t))
                result.lines.append(t);
        }
    }
    return result;
}

// one axis of resizing: the side opposite to the handle stays, the other follows
// side is -1 or 1 for the handle's side, 0 if this axis isn't touched
static void setSpan(int side, double startLen, double wantLen, double &lo, double &hi)
{
    lo = -startLen / 2;
    hi = startLen / 2;
    if (!side)
        return;

    double anchor = -side * startLen / 2;
    double edge = anchor + side * wantLen;
    lo = qMin(anchor, edge);
    hi = qMax(anchor, edge);
}

QString TransformTool::name() const
{
    return "Transform";
}

// cursor follows whatever handle is hovered or being dragged
Qt::CursorShape TransformTool::cursor() const
{
    H::Handle h = dragging != H::None ? dragging : hover;
    switch (h) {
    case H::Body:                          return Qt::SizeAllCursor;
    case H::Rotate:                        return Qt::PointingHandCursor;
    case H::TopLeft: case H::BottomRight:  return Qt::SizeFDiagCursor;
    case H::TopRight: case H::BottomLeft:  return Qt::SizeBDiagCursor;
    case H::Top: case H::Bottom:           return Qt::SizeVerCursor;
    case H::Left: case H::Right:           return Qt::SizeHorCursor;
    default:                               return Qt::ArrowCursor;
    }
}

void TransformTool::activated()
{
    if (!view)
        return;

    // handles have to be resized when zooming, and the selection dies with the page
    zoomConnection = connect(view, &CanvasView::zoomChanged, this, &TransformTool::onZoomChanged);
    resetConnection = connect(view, &CanvasView::pageAboutToReset, this, [this]() {
        clearGuides();
        selected = nullptr;
        dragging = H::None;
        updateFields();
    });
}

void TransformTool::deactivated()
{
    disconnect(zoomConnection);
    disconnect(resetConnection);
    clearGuides();
    select(nullptr);
    hover = H::None;
    dragging = H::None;
}

QWidget *TransformTool::createOptionsWidget()
{
    QWidget *w = new QWidget;

    xBox = new QSpinBox;
    yBox = new QSpinBox;
    widthBox = new QSpinBox;
    heightBox = new QSpinBox;
    for (QSpinBox *box : {xBox, yBox}) {
        box->setRange(-100000, 100000);
        box->setSuffix(" px");
    }
    for (QSpinBox *box : {widthBox, heightBox}) {
        box->setRange(20, 100000);
        box->setSuffix(" px");
    }

    rotationBox = new QDoubleSpinBox;
    rotationBox->setRange(0, 359.9);
    rotationBox->setDecimals(1);
    rotationBox->setWrapping(true);
    rotationBox->setSuffix("°");

    QFormLayout *form = new QFormLayout;
    form->setContentsMargins(0, 0, 0, 0);
    form->addRow("X (center)", xBox);
    form->addRow("Y (center)", yBox);
    form->addRow("Width", widthBox);
    form->addRow("Height", heightBox);
    form->addRow("Rotation", rotationBox);

    snapBox = new QCheckBox("Snap to page and boxes");
    snapBox->setChecked(snapping);

    QLabel *hint = new QLabel("Shift on a corner keeps the proportions, Shift while rotating snaps to 15°. "
                              "Hold Ctrl to turn snapping off for a moment, Delete removes the box.");
    hint->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(form);
    layout->addWidget(snapBox);
    layout->addWidget(hint);
    layout->addStretch();

    for (QSpinBox *box : {xBox, yBox, widthBox, heightBox})
        connect(box, &QSpinBox::valueChanged, this, &TransformTool::applyFields);
    connect(rotationBox, &QDoubleSpinBox::valueChanged, this, &TransformTool::applyFields);
    connect(snapBox, &QCheckBox::toggled, this, [this](bool on) { snapping = on; });

    updateFields();
    return w;
}

void TransformTool::select(ParagraphItem *item)
{
    if (selected != item) {
        if (selected)
            selected->setActive(false);
        selected = item;
        if (selected) {
            selected->setPixelScale(1.0 / view->transform().m11());
            selected->setActive(true);
        }
    }
    updateFields();
}

void TransformTool::deleteSelected()
{
    ParagraphItem *doomed = selected;
    select(nullptr);   // also greys out the spin boxes
    hover = H::None;
    clearGuides();
    delete doomed;     // takes itself out of the scene

    emit cursorChanged();
    emit status("Deleted the box");   // TODO make this undoable in the undo stage
}

bool TransformTool::keyPressed(int key)
{
    if (key == Qt::Key_Delete && selected && dragging == H::None) {
        deleteSelected();
        return true;
    }

    // backspace as well? the text tool will want it, so leaving it off for now
    // if (key == Qt::Key_Backspace && selected) { deleteSelected(); return true; }
    return false;
}

void TransformTool::onZoomChanged()
{
    if (selected)
        selected->setPixelScale(1.0 / view->transform().m11());
}

void TransformTool::startDrag(H::Handle handle, const QPointF &pagePos)
{
    dragging = handle;
    pressPos = pagePos;
    startCenter = selected->pos();
    startSize = selected->size();
    startRotation = selected->rotation();
    emit cursorChanged();
}

void TransformTool::mousePressed(const QPointF &scenePos)
{
    PageItem *page = view ? view->pageItem() : nullptr;
    if (!page)
        return;
    QPointF pagePos = page->mapFromScene(scenePos);

    // grabbing a handle of the box that is already selected
    if (selected) {
        H::Handle h = selected->handleAt(selected->mapFromScene(scenePos));
        if (h != H::None) {
            startDrag(h, pagePos);
            return;
        }
    }

    // otherwise pick whatever box is under the mouse, nothing deselects
    ParagraphItem *hit = view->paragraphAt(scenePos);
    select(hit);
    if (hit) {
        startDrag(H::Body, pagePos);
        emit status(QString("Selected a box (%1 x %2 px)")
                        .arg(qRound(hit->size().width())).arg(qRound(hit->size().height())));
    }
}

void TransformTool::mouseMoved(const QPointF &scenePos)
{
    PageItem *page = view ? view->pageItem() : nullptr;
    if (!page)
        return;

    // not dragging, only update the cursor
    if (dragging == H::None) {
        H::Handle h = selected ? selected->handleAt(selected->mapFromScene(scenePos)) : H::None;
        if (h != hover) {
            hover = h;
            emit cursorChanged();
        }
        return;
    }

    QPointF p = page->mapFromScene(scenePos);

    if (dragging == H::Body) {
        selected->setPos(startCenter + (p - pressPos));
        if (snapOn())
            snapMove();
        else
            clearGuides();
    } else if (dragging == H::Rotate) {
        // the knob is above the box, so 0 degrees is straight up
        QPointF v = p - startCenter;
        double angle = qRadiansToDegrees(qAtan2(v.x(), -v.y()));
        if (shiftHeld())
            angle = qRound(angle / 15.0) * 15.0;
        selected->setRotation(angle);
    } else {
        resizeTo(p);
    }

    updateFields();
    emit status(QString("%1 x %2 px, rotated %3°")
                    .arg(qRound(selected->size().width()))
                    .arg(qRound(selected->size().height()))
                    .arg(qRound(selected->rotation())));
}

void TransformTool::mouseReleased(const QPointF &)
{
    dragging = H::None;
    clearGuides();
    emit cursorChanged();
}

// moves the dragged handle to the mouse, the opposite side stays put
void TransformTool::resizeTo(QPointF p)
{
    // which sides this handle moves: -1, 0 or 1
    int sx = 0, sy = 0;
    switch (dragging) {
    case H::TopLeft:     sx = -1; sy = -1; break;
    case H::Top:         sy = -1; break;
    case H::TopRight:    sx = 1;  sy = -1; break;
    case H::Right:       sx = 1;  break;
    case H::BottomRight: sx = 1;  sy = 1;  break;
    case H::Bottom:      sy = 1;  break;
    case H::BottomLeft:  sx = -1; sy = 1;  break;
    case H::Left:        sx = -1; break;
    default: return;
    }

    // shift only does something on the corners
    bool proportional = sx && sy && shiftHeld();

    // snapping the edge only makes sense while the box isn't rotated,
    // and it would fight with the proportions so that skips it too
    QList<double> guideX, guideY;
    bool unrotated = qAbs(std::fmod(startRotation, 360.0)) < 0.01;
    if (snapOn() && unrotated && !proportional) {
        QList<double> targetsX, targetsY;
        collectTargets(targetsX, targetsY);
        double threshold = snapThreshold();

        if (sx) {
            SnapResult r = snapAxis({p.x()}, targetsX, threshold);
            p.rx() += r.shift;
            guideX = r.lines;
        }
        if (sy) {
            SnapResult r = snapAxis({p.y()}, targetsY, threshold);
            p.ry() += r.shift;
            guideY = r.lines;
        }
    }
    showGuides(guideX, guideY);

    // mouse in the box's own frame from when the drag started (not rotated, center at 0,0)
    QPointF m = QTransform().rotate(-startRotation).map(p - startCenter);

    // how big the box wants to be, measured from the opposite side
    const double minSize = 20;
    double wantW = sx ? qMax(minSize, sx * (m.x() + sx * startSize.width() / 2)) : startSize.width();
    double wantH = sy ? qMax(minSize, sy * (m.y() + sy * startSize.height() / 2)) : startSize.height();

    if (proportional) {
        // whichever side changed more wins, the other one follows
        double f = qMax(wantW / startSize.width(), wantH / startSize.height());
        f = qMax(f, qMax(minSize / startSize.width(), minSize / startSize.height()));
        wantW = startSize.width() * f;
        wantH = startSize.height() * f;
    }

    double x0, x1, y0, y1;
    setSpan(sx, startSize.width(), wantW, x0, x1);
    setSpan(sy, startSize.height(), wantH, y0, y1);

    // new center goes back to page coordinates through the rotation
    QPointF localCenter((x0 + x1) / 2, (y0 + y1) / 2);
    selected->setSize(QSizeF(x1 - x0, y1 - y0));
    selected->setPos(startCenter + QTransform().rotate(startRotation).map(localCenter));
}

bool TransformTool::snapOn() const
{
    return snapping && !ctrlHeld();
}

// distance in scene units that counts as close enough, 8 screen pixels at any zoom
double TransformTool::snapThreshold() const
{
    // return 8;   // old fixed one, way too grabby when zoomed out
    return 8.0 / view->transform().m11();
}

// every line something can snap to, in page coordinates: the page and all the other boxes
void TransformTool::collectTargets(QList<double> &xs, QList<double> &ys) const
{
    QRectF page = view->pageItem()->boundingRect();
    xs = {page.left(), page.center().x(), page.right()};
    ys = {page.top(), page.center().y(), page.bottom()};

    for (ParagraphItem *other : view->paragraphs()) {
        if (other == selected)
            continue;
        QRectF r = other->pageRect();
        xs << r.left() << r.center().x() << r.right();
        ys << r.top() << r.center().y() << r.bottom();
    }
}

// nudges the box being moved onto the closest lines and shows them
void TransformTool::snapMove()
{
    QList<double> targetsX, targetsY;
    collectTargets(targetsX, targetsY);

    QRectF r = selected->pageRect();
    double threshold = snapThreshold();
    SnapResult sx = snapAxis({r.left(), r.center().x(), r.right()}, targetsX, threshold);
    SnapResult sy = snapAxis({r.top(), r.center().y(), r.bottom()}, targetsY, threshold);

    selected->setPos(selected->pos() + QPointF(sx.shift, sy.shift));
    showGuides(sx.lines, sy.lines);
}

// replaces whatever guides are showing, empty lists just remove them
void TransformTool::showGuides(const QList<double> &xs, const QList<double> &ys)
{
    clearGuides();

    PageItem *page = view ? view->pageItem() : nullptr;
    if (!page)
        return;

    QRectF r = page->boundingRect();
    QPen pen(Theme::guide(), 1);
    pen.setCosmetic(true);   // 1px at any zoom

    auto add = [&](const QLineF &line) {
        QGraphicsLineItem *item = new QGraphicsLineItem(line, page);
        item->setPen(pen);
        item->setZValue(1000);   // above the boxes
        guides.append(item);
    };

    for (double x : xs)
        add(QLineF(x, r.top(), x, r.bottom()));
    for (double y : ys)
        add(QLineF(r.left(), y, r.right(), y));
}

void TransformTool::clearGuides()
{
    qDeleteAll(guides);
    guides.clear();
}

// shows the selected box in the spin boxes, greyed out when nothing is selected
void TransformTool::updateFields()
{
    if (!xBox)
        return;

    const QList<QWidget *> fields = {xBox, yBox, widthBox, heightBox, rotationBox};
    for (QWidget *f : fields) {
        f->setEnabled(selected != nullptr);
        f->blockSignals(true);   // so setting the values doesn't trigger applyFields
    }

    if (selected) {
        xBox->setValue(qRound(selected->x()));
        yBox->setValue(qRound(selected->y()));
        widthBox->setValue(qRound(selected->size().width()));
        heightBox->setValue(qRound(selected->size().height()));

        double r = std::fmod(selected->rotation(), 360.0);
        if (r < 0)
            r += 360.0;
        rotationBox->setValue(r);
    }

    for (QWidget *f : fields)
        f->blockSignals(false);
}

void TransformTool::applyFields()
{
    if (!selected)
        return;

    selected->setSize(QSizeF(widthBox->value(), heightBox->value()));
    selected->setPos(xBox->value(), yBox->value());
    selected->setRotation(rotationBox->value());
}
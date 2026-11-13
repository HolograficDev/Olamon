#include "canvasview.h"
#include "document.h"
#include "pageitem.h"
#include "tool.h"
#include <QEnterEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>
#include <cmath>
#include "theme.h"
#include "paragraphitem.h"

CanvasView::CanvasView(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    setScene(scene);

    setBackgroundBrush(Theme::workspace());
    setFrameShape(QFrame::NoFrame);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setFocusPolicy(Qt::ClickFocus);
}

void CanvasView::setDocument(Document *document)
{
    buildPage(document->settings());

    // changing file options later rebuilds the page
    connect(document, &Document::settingsChanged, this, [this, document]() {
        buildPage(document->settings());
        fitPage();
    });

    // wait a tick so the view has its real size before fitting
    QTimer::singleShot(0, this, &CanvasView::fitPage);
}
void CanvasView::setTool(Tool *tool)
{
    if (activeTool) {
        activeTool->deactivated();
        disconnect(activeTool, &Tool::cursorChanged, this, &CanvasView::updateCursor);
    }
    activeTool = tool;
    if (activeTool) {
        activeTool->activated();
        connect(activeTool, &Tool::cursorChanged, this, &CanvasView::updateCursor);
    }
    updateCursor();
}

// hand cursors win over the tool cursor while panning
void CanvasView::updateCursor()
{
    if (panning)
        viewport()->setCursor(Qt::ClosedHandCursor);
    else if (spaceDown)
        viewport()->setCursor(Qt::OpenHandCursor);
    else if (activeTool)
        viewport()->setCursor(activeTool->cursor());
    else
        viewport()->unsetCursor();
}
ParagraphItem *CanvasView::paragraphAt(const QPointF &scenePos) const
{
    // items() gives the top one first
    for (QGraphicsItem *item : scene->items(scenePos)) {
        if (ParagraphItem *p = qgraphicsitem_cast<ParagraphItem *>(item))
            return p;
    }
    return nullptr;
}
QList<ParagraphItem *> CanvasView::paragraphs() const
{
    QList<ParagraphItem *> list;
    if (!page)
        return list;

    for (QGraphicsItem *item : page->childItems()) {
        if (ParagraphItem *p = qgraphicsitem_cast<ParagraphItem *>(item))
            list.append(p);
    }
    return list;
}
void CanvasView::buildPage(const CanvasSettings &settings)
{
    emit pageAboutToReset();
    scene->clear();   // deletes the old page too

    page = new PageItem(QSize(settings.width, settings.height), settings.background);
    scene->addItem(page);

    // extra room around the page so you can pan past the edges
    QRectF r = page->boundingRect();
    scene->setSceneRect(r.adjusted(-r.width() / 2, -r.height() / 2, r.width() / 2, r.height() / 2));
}

double CanvasView::currentZoom() const
{
    return transform().m11();
}

void CanvasView::fitPage()
{
    if (!page)
        return;
    fitInView(page->boundingRect().adjusted(-40, -40, 40, 40), Qt::KeepAspectRatio);
    emit zoomChanged(currentZoom());
}

void CanvasView::actualSize()
{
    resetTransform();
    if (page)
        centerOn(page);
    emit zoomChanged(currentZoom());
}

void CanvasView::zoomBy(double factor)
{
    double current = currentZoom();
    double target = qBound(0.02, current * factor, 10.0);

    scale(target / current, target / current);
    emit zoomChanged(target);
}

// ctrl + wheel zooms, plain wheel scrolls like normal
void CanvasView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        zoomBy(std::pow(1.0015, event->angleDelta().y()));
        event->accept();
        return;
    }
    QGraphicsView::wheelEvent(event);
}

// middle mouse or space + left mouse drags the view around
void CanvasView::mousePressEvent(QMouseEvent *event)
{
    bool wantsPan = event->button() == Qt::MiddleButton
                    || (spaceDown && event->button() == Qt::LeftButton);
    if (wantsPan) {
        panning = true;
        lastPanPos = event->pos();
        updateCursor();
        event->accept();
        return;
    }

    if (activeTool && event->button() == Qt::LeftButton)
        activeTool->mousePressed(mapToScene(event->pos()));
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
    if (panning) {
        QPoint delta = event->pos() - lastPanPos;
        lastPanPos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
        return;
    }

    if (activeTool)
        activeTool->mouseMoved(mapToScene(event->pos()));
    QGraphicsView::mouseMoveEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
    if (panning && (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton)) {
        panning = false;
        updateCursor();
        event->accept();
        return;
    }

    if (activeTool && event->button() == Qt::LeftButton)
        activeTool->mouseReleased(mapToScene(event->pos()));
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::keyPressEvent(QKeyEvent *event)
{
    // a text box is being edited, so space is just a space
    bool typing = scene->focusItem() != nullptr;

    if (!typing && event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        spaceDown = true;
        updateCursor();
        return;
    }

    // tools get keys too, delete in the transform tool for example
    if (activeTool && activeTool->keyPressed(event->key())) {
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

void CanvasView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        spaceDown = false;
        updateCursor();
        return;
    }
    QGraphicsView::keyReleaseEvent(event);
}
// grab focus when the mouse is over the canvas so space works without clicking first
void CanvasView::enterEvent(QEnterEvent *event)
{
    setFocus();
    QGraphicsView::enterEvent(event);
}
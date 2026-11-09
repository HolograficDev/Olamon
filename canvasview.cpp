#include "canvasview.h"
#include "document.h"
#include "pageitem.h"

#include <QEnterEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>
#include <cmath>

CanvasView::CanvasView(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    setScene(scene);

    setBackgroundBrush(QColor("#3c3c3c"));
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

void CanvasView::buildPage(const CanvasSettings &settings)
{
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
        viewport()->setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
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
    QGraphicsView::mouseMoveEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
    if (panning && (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton)) {
        panning = false;
        if (spaceDown)
            viewport()->setCursor(Qt::OpenHandCursor);
        else
            viewport()->unsetCursor();
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        spaceDown = true;
        viewport()->setCursor(Qt::OpenHandCursor);
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

void CanvasView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        spaceDown = false;
        if (!panning)
            viewport()->unsetCursor();
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
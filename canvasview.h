#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>
#include <QPoint>

class Document;
class PageItem;
struct CanvasSettings;

class CanvasView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CanvasView(QWidget *parent = nullptr);

    void setDocument(Document *document);

public slots:
    void fitPage();      // whole page visible
    void actualSize();   // 100%

signals:
    void zoomChanged(double zoom);   // 1.0 = 100%

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void enterEvent(QEnterEvent *event) override;

private:
    void buildPage(const CanvasSettings &settings);
    void zoomBy(double factor);
    double currentZoom() const;

    QGraphicsScene *scene;
    PageItem *page = nullptr;

    bool panning = false;
    bool spaceDown = false;
    QPoint lastPanPos;
};

#endif // CANVASVIEW_H
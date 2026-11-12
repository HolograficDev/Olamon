#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QPointF>
#include <Qt>

class QWidget;
class CanvasView;

// base class for everything in the tool panel
class Tool : public QObject
{
    Q_OBJECT

public:
    explicit Tool(QObject *parent = nullptr);

    virtual QString name() const = 0;
    virtual Qt::CursorShape cursor() const;

    // tools need the canvas to add things to the page
    void setView(CanvasView *canvasView);

    // the settings shown in the options panel, made the first time it's asked for
    QWidget *optionsWidget();

    virtual void activated();
    virtual void deactivated();

    // positions are in scene coordinates
    virtual void mousePressed(const QPointF &scenePos);
    virtual void mouseMoved(const QPointF &scenePos);
    virtual void mouseReleased(const QPointF &scenePos);
    // return true if the tool used the key
    virtual bool keyPressed(int key);
signals:
    void status(const QString &text);   // shows in the status bar
    void cursorChanged();               // tool wants the canvas cursor refreshed

protected:
    virtual QWidget *createOptionsWidget() = 0;

    CanvasView *view = nullptr;

private:
    QWidget *options = nullptr;
};

#endif // TOOL_H
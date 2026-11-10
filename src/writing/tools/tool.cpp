#include "tool.h"

#include <QWidget>

Tool::Tool(QObject *parent)
    : QObject(parent)
{
}

Qt::CursorShape Tool::cursor() const
{
    return Qt::ArrowCursor;
}

QWidget *Tool::optionsWidget()
{
    if (!options)
        options = createOptionsWidget();
    return options;
}

// the mouse ones do nothing unless a tool overrides them
void Tool::activated() {}
void Tool::deactivated() {}
void Tool::mousePressed(const QPointF &) {}
void Tool::mouseMoved(const QPointF &) {}
void Tool::mouseReleased(const QPointF &) {}
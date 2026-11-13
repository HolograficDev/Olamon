#ifndef DRAWPARAGRAPHTOOL_H
#define DRAWPARAGRAPHTOOL_H

#include "tool.h"

#include <QColor>
#include <QPointF>

class ParagraphItem;
class QPushButton;

class DrawParagraphTool : public Tool
{
    Q_OBJECT

public:
    using Tool::Tool;

    QString name() const override;
    Qt::CursorShape cursor() const override;

    void mousePressed(const QPointF &scenePos) override;
    void mouseMoved(const QPointF &scenePos) override;
    void mouseReleased(const QPointF &scenePos) override;

protected:
    QWidget *createOptionsWidget() override;

private:
    //void updateColorButton();
    QColor currentColor() const;

    QPushButton *colorButton = nullptr;
    QColor boxColor = Qt::white;
    bool transparent = true;
    int textSize = 48;

    ParagraphItem *current = nullptr;   // the box being dragged out
    QPointF start;                      // in page coordinates
};

#endif // DRAWPARAGRAPHTOOL_H
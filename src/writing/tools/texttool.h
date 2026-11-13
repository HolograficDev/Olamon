#ifndef TEXTTOOL_H
#define TEXTTOOL_H

#include "tool.h"

#include <QMetaObject>

class ParagraphItem;
class QButtonGroup;

class TextTool : public Tool
{
    Q_OBJECT

public:
    using Tool::Tool;

    QString name() const override;
    Qt::CursorShape cursor() const override;

    void activated() override;
    void deactivated() override;

    void mousePressed(const QPointF &scenePos) override;
    bool keyPressed(int key) override;

protected:
    QWidget *createOptionsWidget() override;

private:
    void startEditing(ParagraphItem *item);
    void stopEditing();
    void updateButtons();   // box -> alignment buttons

    ParagraphItem *editing = nullptr;
    QButtonGroup *alignGroup = nullptr;
    QMetaObject::Connection resetConnection;
};

#endif // TEXTTOOL_H
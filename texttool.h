#ifndef TEXTTOOL_H
#define TEXTTOOL_H

#include "tool.h"

class TextTool : public Tool
{
    Q_OBJECT

public:
    using Tool::Tool;

    QString name() const override;
    Qt::CursorShape cursor() const override;

protected:
    QWidget *createOptionsWidget() override;
};

#endif // TEXTTOOL_H
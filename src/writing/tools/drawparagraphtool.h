#ifndef DRAWPARAGRAPHTOOL_H
#define DRAWPARAGRAPHTOOL_H

#include "tool.h"

class DrawParagraphTool : public Tool
{
    Q_OBJECT

public:
    using Tool::Tool;

    QString name() const override;
    Qt::CursorShape cursor() const override;

protected:
    QWidget *createOptionsWidget() override;
};

#endif // DRAWPARAGRAPHTOOL_H
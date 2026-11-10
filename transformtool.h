#ifndef TRANSFORMTOOL_H
#define TRANSFORMTOOL_H

#include "tool.h"

class TransformTool : public Tool
{
    Q_OBJECT

public:
    using Tool::Tool;

    QString name() const override;

protected:
    QWidget *createOptionsWidget() override;
};

#endif // TRANSFORMTOOL_H
#include "transformtool.h"

#include <QLabel>
#include <QVBoxLayout>

QString TransformTool::name() const
{
    return "Transform";
}

QWidget *TransformTool::createOptionsWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *hint = new QLabel("position, size and rotation will go here");
    hint->setWordWrap(true);
    layout->addWidget(hint);
    layout->addStretch();
    return w;
}
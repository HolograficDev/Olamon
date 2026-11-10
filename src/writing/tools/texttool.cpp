#include "texttool.h"

#include <QLabel>
#include <QVBoxLayout>

QString TextTool::name() const
{
    return "Text Tool";
}

Qt::CursorShape TextTool::cursor() const
{
    return Qt::IBeamCursor;
}

QWidget *TextTool::createOptionsWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *hint = new QLabel("font, size, bold/italic, color and reading will go here");
    hint->setWordWrap(true);
    layout->addWidget(hint);
    layout->addStretch();
    return w;
}
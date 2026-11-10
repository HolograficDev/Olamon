#include "drawparagraphtool.h"

#include <QLabel>
#include <QVBoxLayout>

QString DrawParagraphTool::name() const
{
    return "Draw Paragraph";
}

Qt::CursorShape DrawParagraphTool::cursor() const
{
    return Qt::CrossCursor;
}

QWidget *DrawParagraphTool::createOptionsWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *hint = new QLabel("box color and default text size will go here");
    hint->setWordWrap(true);
    layout->addWidget(hint);
    layout->addStretch();
    return w;
}
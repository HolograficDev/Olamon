#include "writingtab.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QToolButton>
#include <QVBoxLayout>

WritingTab::WritingTab(QWidget *parent)
    : QWidget(parent)
{
    // left side: tools on top, options below
    QSplitter *leftSide = new QSplitter(Qt::Vertical);
    leftSide->addWidget(createToolPanel());
    leftSide->addWidget(createOptionsPanel());
    leftSide->setStretchFactor(0, 3);
    leftSide->setStretchFactor(1, 2);
    leftSide->setChildrenCollapsible(false);
    leftSide->setFixedWidth(240);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(leftSide);
    layout->addWidget(createCanvasArea(), 1);
}

// top part of the left panel
QWidget *WritingTab::createToolPanel()
{
    QFrame *panel = new QFrame;
    panel->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(8, 8, 8, 8);

    //buttons went black and unreadble
    panel->setStyleSheet(
        "QToolButton { border: 1px solid palette(mid); border-radius: 3px; padding: 4px; } "
        "QToolButton:hover { background: palette(midlight); } "
        "QToolButton:checked { background: palette(highlight); color: palette(highlighted-text); }");

    toolGroup = new QButtonGroup(this);
    toolGroup->setExclusive(true);

    // just placeholders for now, real tools come later
    QStringList tools = {"Draw Paragraph", "Text Tool", "Transform"};
    for (const QString &name : tools) {
        QToolButton *btn = new QToolButton;
        btn->setText(name);
        btn->setCheckable(true);
        btn->setMinimumHeight(36);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        toolGroup->addButton(btn);
        layout->addWidget(btn);
    }
    layout->addStretch();

    connect(toolGroup, &QButtonGroup::buttonClicked, this, &WritingTab::onToolPicked);

    toolGroup->buttons().first()->setChecked(true);
    return panel;
}

// bottom part, shows settings of the picked tool
QWidget *WritingTab::createOptionsPanel()
{
    QFrame *panel = new QFrame;
    panel->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(8, 8, 8, 8);

    optionsTitle = new QLabel;
    optionsTitle->setStyleSheet("font-weight: bold;");
    layout->addWidget(optionsTitle);

    QLabel *hint = new QLabel("tool settings will show up here");
    hint->setWordWrap(true);
    layout->addWidget(hint);
    layout->addStretch();

    onToolPicked(toolGroup->checkedButton());
    return panel;
}

// right side, the canvas goes here later
QWidget *WritingTab::createCanvasArea()
{
    QFrame *area = new QFrame;
    area->setObjectName("canvasArea");
    area->setStyleSheet("#canvasArea { background: #3c3c3c; }");

    QVBoxLayout *layout = new QVBoxLayout(area);
    QLabel *label = new QLabel("no canvas yet");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: #aaaaaa;");
    layout->addWidget(label);
    return area;
}

void WritingTab::onToolPicked(QAbstractButton *button)
{
    optionsTitle->setText(button->text() + " options");
}
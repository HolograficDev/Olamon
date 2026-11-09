#include "writingtab.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QSplitter>
#include <QToolButton>
#include <QVBoxLayout>

// draws a tool icon in code, swap this for svg files later
static QPixmap drawToolIcon(int type, const QColor &color)
{
    const int size = 28;
    QPixmap pix(size * 2, size * 2);
    pix.setDevicePixelRatio(2);   // keeps it sharp on high dpi
    pix.fill(Qt::transparent);

    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(color, 2);
    p.setPen(pen);

    if (type == 0) {
        // paragraph, dashed box
        pen.setStyle(Qt::DashLine);
        p.setPen(pen);
        p.drawRect(QRect(4, 7, 20, 14));
    } else if (type == 1) {
        // text, a big T
        QFont f("Serif");
        f.setPixelSize(22);
        f.setBold(true);
        p.setFont(f);
        p.drawText(QRect(0, 0, size, size), Qt::AlignCenter, "T");
    } else {
        // transform, box with corner handles
        p.drawRect(QRect(8, 8, 12, 12));
        p.setBrush(color);
        for (int x : {8, 20})
            for (int y : {8, 20})
                p.drawRect(QRect(x - 3, y - 3, 6, 6));
    }
    return pix;
}

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

// top part of the left panel, grid of icon buttons + status box
QWidget *WritingTab::createToolPanel()
{
    QFrame *panel = new QFrame;
    panel->setFrameShape(QFrame::StyledPanel);

    // explicit look for the buttons, the default checked look goes black
    panel->setStyleSheet(
        "QToolButton { border: 1px solid palette(mid); border-radius: 3px; } "
        "QToolButton:hover { background: palette(midlight); } "
        "QToolButton:checked { background: palette(highlight); }");

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(8, 8, 8, 8);

    toolGroup = new QButtonGroup(this);
    toolGroup->setExclusive(true);

    QGridLayout *grid = new QGridLayout;
    grid->setSpacing(6);

    // placeholders for now, real tools come later
    QStringList tools = {"Draw Paragraph", "Text Tool", "Transform"};
    const int columns = 4;

    for (int i = 0; i < tools.size(); ++i) {
        QToolButton *btn = new QToolButton;
        btn->setToolTip(tools[i]);
        btn->setCheckable(true);
        btn->setFixedSize(44, 44);
        btn->setIconSize(QSize(28, 28));

        // normal icon uses text color, checked one uses the highlight text color
        QIcon icon;
        QPixmap normal = drawToolIcon(i, palette().color(QPalette::ButtonText));
        QPixmap checked = drawToolIcon(i, palette().color(QPalette::HighlightedText));
        icon.addPixmap(normal, QIcon::Normal, QIcon::Off);
        icon.addPixmap(normal, QIcon::Active, QIcon::Off);
        icon.addPixmap(checked, QIcon::Normal, QIcon::On);
        icon.addPixmap(checked, QIcon::Active, QIcon::On);
        btn->setIcon(icon);

        toolGroup->addButton(btn);
        grid->addWidget(btn, i / columns, i % columns);
    }
    grid->setColumnStretch(columns, 1);   // keeps buttons packed to the left

    layout->addLayout(grid);
    layout->addStretch();

    // status box at the bottom
    statusLabel = new QLabel;
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("padding: 4px; border: 1px solid palette(mid); border-radius: 3px;");
    layout->addWidget(statusLabel);

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
    optionsTitle->setText(button->toolTip() + " options");
    setStatus("Tool: " + button->toolTip());
}

void WritingTab::setStatus(const QString &text)
{
    statusLabel->setText(text);
}
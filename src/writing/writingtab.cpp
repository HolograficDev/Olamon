#include "writingtab.h"
#include "fontmanager.h"
#include "document.h"
#include "newcanvasdialog.h"
#include "canvasview.h"
#include <QStackedWidget>
#include <QPushButton>
#include <QTimer>
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
#include "drawparagraphtool.h"
#include "texttool.h"
#include "transformtool.h"
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
    // tools need the canvas, and report back to the status box
    for (Tool *tool : tools) {
        tool->setView(canvasView);
        connect(tool, &Tool::status, this, &WritingTab::setStatus);
    }
    setTool(0);
    QTimer::singleShot(0, this, &WritingTab::newCanvas);
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

    // order here is the order of the buttons, the icons in drawToolIcon match it
    tools = {new DrawParagraphTool(this), new TextTool(this), new TransformTool(this)};

    toolGroup = new QButtonGroup(this);
    toolGroup->setExclusive(true);

    QGridLayout *grid = new QGridLayout;
    grid->setSpacing(6);
    const int columns = 4;

    for (int i = 0; i < tools.size(); ++i) {
        QToolButton *btn = new QToolButton;
        btn->setToolTip(tools[i]->name());
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

        toolGroup->addButton(btn, i);
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

    connect(toolGroup, &QButtonGroup::idClicked, this, &WritingTab::setTool);
    toolGroup->button(0)->setChecked(true);
    return panel;
}

// bottom part, shows the settings of the picked tool
QWidget *WritingTab::createOptionsPanel()
{
    QFrame *panel = new QFrame;
    panel->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(8, 8, 8, 8);

    optionsTitle = new QLabel;
    optionsTitle->setStyleSheet("font-weight: bold;");
    layout->addWidget(optionsTitle);

    optionsStack = new QStackedWidget;
    for (Tool *tool : tools)
        optionsStack->addWidget(tool->optionsWidget());
    layout->addWidget(optionsStack, 1);

    return panel;
}

// right side, the canvas goes here later
// right side, either the empty screen or the canvas with its bottom bar
QWidget *WritingTab::createCanvasArea()
{
    QFrame *area = new QFrame;
    area->setObjectName("canvasArea");
    area->setStyleSheet("#canvasArea { background: #3c3c3c; } "
                        "#canvasArea QLabel { color: #aaaaaa; }");

    canvasStack = new QStackedWidget;

    // page 0, nothing created yet
    QWidget *empty = new QWidget;
    QLabel *emptyLabel = new QLabel("no canvas yet");
    emptyLabel->setAlignment(Qt::AlignCenter);
    QPushButton *emptyButton = new QPushButton("New Canvas...");
    connect(emptyButton, &QPushButton::clicked, this, &WritingTab::newCanvas);

    QVBoxLayout *emptyLayout = new QVBoxLayout(empty);
    emptyLayout->addStretch();
    emptyLayout->addWidget(emptyLabel);
    emptyLayout->addWidget(emptyButton, 0, Qt::AlignHCenter);
    emptyLayout->addStretch();
    canvasStack->addWidget(empty);

    // page 1, the canvas and a small bar under it
    QWidget *editor = new QWidget;
    canvasView = new CanvasView;

    QPushButton *newButton = new QPushButton("New...");
    zoomLabel = new QLabel("100%");
    QPushButton *fitButton = new QPushButton("Fit");
    QPushButton *actualButton = new QPushButton("100%");

    QHBoxLayout *bar = new QHBoxLayout;
    bar->setContentsMargins(6, 4, 6, 4);
    bar->addWidget(newButton);
    bar->addStretch();
    bar->addWidget(zoomLabel);
    bar->addWidget(fitButton);
    bar->addWidget(actualButton);

    QVBoxLayout *editorLayout = new QVBoxLayout(editor);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(0);
    editorLayout->addWidget(canvasView, 1);
    editorLayout->addLayout(bar);
    canvasStack->addWidget(editor);

    connect(newButton, &QPushButton::clicked, this, &WritingTab::newCanvas);
    connect(fitButton, &QPushButton::clicked, canvasView, &CanvasView::fitPage);
    connect(actualButton, &QPushButton::clicked, canvasView, &CanvasView::actualSize);
    connect(canvasView, &CanvasView::zoomChanged, this, [this](double zoom) {
        zoomLabel->setText(QString::number(qRound(zoom * 100)) + "%");
    });

    QVBoxLayout *areaLayout = new QVBoxLayout(area);
    areaLayout->setContentsMargins(0, 0, 0, 0);
    areaLayout->addWidget(canvasStack);
    return area;
}

void WritingTab::setTool(int index)
{
    Tool *tool = tools[index];
    optionsTitle->setText(tool->name() + " options");
    optionsStack->setCurrentIndex(index);
    canvasView->setTool(tool);
    setStatus("Tool: " + tool->name());
}

void WritingTab::setStatus(const QString &text)
{
    statusLabel->setText(text);
}
void WritingTab::newCanvas()
{
    NewCanvasDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    // TODO ask to save the old one first once saving exists
    delete document;
    document = new Document(dialog.settings(), this);

    canvasStack->setCurrentIndex(1);
    canvasView->setDocument(document);

    const CanvasSettings &s = document->settings();
    setStatus(QString("Created canvas %1 (%2 x %3 px)").arg(s.name).arg(s.width).arg(s.height));
}
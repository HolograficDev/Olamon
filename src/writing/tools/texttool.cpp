#include "texttool.h"
#include "canvasview.h"
#include "paragraphitem.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// same order as the buttons below
static const Qt::Alignment alignments[] = {Qt::AlignLeft, Qt::AlignHCenter, Qt::AlignRight, Qt::AlignJustify};
static const char *alignNames[] = {"Left", "Center", "Right", "Justify"};

QString TextTool::name() const
{
    return "Text Tool";
}

Qt::CursorShape TextTool::cursor() const
{
    return Qt::IBeamCursor;
}

void TextTool::activated()
{
    if (!view)
        return;

    // the box we are typing in is deleted together with the page
    resetConnection = connect(view, &CanvasView::pageAboutToReset, this, [this]() {
        editing = nullptr;
        updateButtons();
    });
}

void TextTool::deactivated()
{
    disconnect(resetConnection);
    stopEditing();
}

QWidget *TextTool::createOptionsWidget()
{
    QWidget *w = new QWidget;

    // alignment of the whole box
    alignGroup = new QButtonGroup(this);
    QGridLayout *grid = new QGridLayout;
    for (int i = 0; i < 4; ++i) {
        QPushButton *btn = new QPushButton(alignNames[i]);
        btn->setCheckable(true);
        alignGroup->addButton(btn, i);
        grid->addWidget(btn, i / 2, i % 2);
    }

    connect(alignGroup, &QButtonGroup::idClicked, this, [this](int id) {
        if (editing)
            editing->setAlignment(alignments[id]);
    });

    QLabel *hint = new QLabel("Click a box and start typing. Esc or clicking outside stops editing. "
                              "Text that doesn't fit is hidden, make the box bigger to see it.");
    hint->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel("Alignment"));
    layout->addLayout(grid);
    layout->addWidget(hint);
    layout->addStretch();

    updateButtons();
    return w;
}

void TextTool::mousePressed(const QPointF &scenePos)
{
    ParagraphItem *hit = view ? view->paragraphAt(scenePos) : nullptr;

    // clicking inside the box we are already typing in, the text item moves the cursor itself
    if (hit == editing)
        return;

    stopEditing();
    if (hit)
        startEditing(hit);
}

bool TextTool::keyPressed(int key)
{
    if (key == Qt::Key_Escape && editing) {
        stopEditing();
        emit status("Stopped editing");
        return true;
    }
    return false;
}

void TextTool::startEditing(ParagraphItem *item)
{
    editing = item;
    editing->setEditing(true);
    updateButtons();
    emit status("Typing in a box, Esc to stop");
}

void TextTool::stopEditing()
{
    if (editing) {
        editing->setEditing(false);
        editing = nullptr;
    }
    updateButtons();
}

// buttons only work with a box being edited, and show how that box is aligned
void TextTool::updateButtons()
{
    if (!alignGroup)
        return;

    for (QAbstractButton *btn : alignGroup->buttons())
        btn->setEnabled(editing != nullptr);

    if (!editing)
        return;

    for (int i = 0; i < 4; ++i) {
        if (alignments[i] == editing->alignment())
            alignGroup->button(i)->setChecked(true);
    }
}
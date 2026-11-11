#include "drawparagraphtool.h"
#include "canvasview.h"
#include "pageitem.h"
#include "paragraphitem.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QSpinBox>
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

    colorButton = new QPushButton;
    colorButton->setDisabled(transparent);
    updateColorButton();

    QCheckBox *transparentBox = new QCheckBox("Transparent");
    transparentBox->setChecked(transparent);

    QSpinBox *sizeBox = new QSpinBox;
    sizeBox->setRange(4, 1000);
    sizeBox->setSuffix(" px");
    sizeBox->setValue(textSize);

    QFormLayout *form = new QFormLayout;
    form->setContentsMargins(0, 0, 0, 0);
    form->addRow("Box color", colorButton);
    form->addRow("", transparentBox);
    form->addRow("Text size", sizeBox);

    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(form);
    layout->addStretch();

    connect(colorButton, &QPushButton::clicked, this, [this, w]() {
        QColor picked = QColorDialog::getColor(boxColor, w, "Box color");
        if (picked.isValid()) {
            boxColor = picked;
            updateColorButton();
        }
    });
    connect(transparentBox, &QCheckBox::toggled, this, [this](bool on) {
        transparent = on;
        colorButton->setDisabled(on);
    });
    connect(sizeBox, &QSpinBox::valueChanged, this, [this](int value) {
        textSize = value;
    });

    return w;
}

void DrawParagraphTool::updateColorButton()
{
    QString textColor = boxColor.lightness() > 128 ? "black" : "white";
    colorButton->setText(boxColor.name().toUpper());
    colorButton->setStyleSheet(QString("background: %1; color: %2; border: 1px solid gray; padding: 4px;")
                                   .arg(boxColor.name(), textColor));
}

QColor DrawParagraphTool::currentColor() const
{
    return transparent ? QColor(0, 0, 0, 0) : boxColor;
}

void DrawParagraphTool::mousePressed(const QPointF &scenePos)
{
    PageItem *page = view ? view->pageItem() : nullptr;
    if (!page)
        return;

    // drags have to start on the page
    QPointF p = page->mapFromScene(scenePos);
    if (!page->boundingRect().contains(p))
        return;

    start = p;
    current = new ParagraphItem(page);
    current->setBoxColor(currentColor());
    current->setTextSize(textSize);
    current->setCorners(start, start);
}

void DrawParagraphTool::mouseMoved(const QPointF &scenePos)
{
    if (!current)
        return;

    current->setCorners(start, current->parentItem()->mapFromScene(scenePos));
    emit status(QString("Box %1 x %2 px")
                    .arg(qRound(current->size().width()))
                    .arg(qRound(current->size().height())));
}

void DrawParagraphTool::mouseReleased(const QPointF &)
{
    if (!current)
        return;

    // tiny boxes are most likely accidental clicks
    QSizeF s = current->size();
    if (s.width() < 20 || s.height() < 20) {
        delete current;
        emit status("Box was too small, drag a bigger one");
    } else {
        emit status(QString("Added a paragraph box (%1 x %2 px)")
                        .arg(qRound(s.width())).arg(qRound(s.height())));
    }
    current = nullptr;
}
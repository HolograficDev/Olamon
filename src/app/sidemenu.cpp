#include "sidemenu.h"

#include <QButtonGroup>
#include <QEvent>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>

SideMenu::SideMenu(QWidget *parent)
    : QWidget(parent)
{
    // watch the parent so we can keep covering it when it resizes
    parent->installEventFilter(this);
    setGeometry(parent->rect());
    hide();

    panel = new QFrame(this);
    panel->setObjectName("panel");
    panel->setStyleSheet(
        "#panel { background: palette(window); border-right: 1px solid palette(mid); } "
        "QPushButton { text-align: left; padding: 10px 16px; border: none; } "
        "QPushButton:hover { background: palette(midlight); } "
        "QPushButton:checked { background: palette(highlight); color: palette(highlighted-text); } "
        "QPushButton[sub=\"true\"] { padding-left: 40px; }");

    list = new QVBoxLayout(panel);
    list->setContentsMargins(0, 8, 0, 8);
    list->setSpacing(0);
    list->addStretch();

    group = new QButtonGroup(this);

    anim = new QPropertyAnimation(panel, "pos", this);
    anim->setDuration(180);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QPropertyAnimation::finished, this, [this]() {
        if (!opened)
            hide();
    });
}

int SideMenu::addMode(const QString &name)
{
    subCount = 0;
    addButton(name, false, modeCount, -1);
    return modeCount++;
}

void SideMenu::addSubTab(const QString &name)
{
    addButton(name, true, modeCount - 1, subCount++);
}

void SideMenu::addButton(const QString &text, bool isSub, int mode, int sub)
{
    QPushButton *btn = new QPushButton(text, panel);
    btn->setCheckable(true);
    btn->setFlat(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("sub", isSub);
    group->addButton(btn);

    // subtabs are faded so they look lower level
    if (isSub) {
        QGraphicsOpacityEffect *fade = new QGraphicsOpacityEffect(btn);
        fade->setOpacity(0.6);
        btn->setGraphicsEffect(fade);
    }

    // first button is the selected one at the start
    if (group->buttons().size() == 1)
        btn->setChecked(true);

    connect(btn, &QPushButton::clicked, this, [this, mode, sub, text, isSub]() {
        emit tabSelected(mode, sub, isSub ? text : QString());
        closeMenu();
    });

    list->insertWidget(list->count() - 1, btn);   // keeps the stretch at the bottom
}

void SideMenu::toggle()
{
    if (opened)
        closeMenu();
    else
        open();
}

void SideMenu::open()
{
    opened = true;
    setGeometry(parentWidget()->rect());
    panel->resize(panelWidth, height());

    if (isHidden())
        panel->move(-panelWidth, 0);
    show();
    raise();

    anim->stop();
    anim->setStartValue(panel->pos());
    anim->setEndValue(QPoint(0, 0));
    anim->start();
}

void SideMenu::closeMenu()
{
    opened = false;
    anim->stop();
    anim->setStartValue(panel->pos());
    anim->setEndValue(QPoint(-panelWidth, 0));
    anim->start();
}

bool SideMenu::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == parentWidget() && event->type() == QEvent::Resize)
        setGeometry(parentWidget()->rect());
    return QWidget::eventFilter(obj, event);
}

// clicking the dimmed area closes the menu
void SideMenu::mousePressEvent(QMouseEvent *event)
{
    if (!panel->geometry().contains(event->pos()))
        closeMenu();
}

void SideMenu::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0, 80));
}

void SideMenu::resizeEvent(QResizeEvent *)
{
    panel->resize(panelWidth, height());
}
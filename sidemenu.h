#ifndef SIDEMENU_H
#define SIDEMENU_H

#include <QWidget>

class QFrame;
class QVBoxLayout;
class QButtonGroup;
class QPushButton;
class QPropertyAnimation;

class SideMenu : public QWidget
{
    Q_OBJECT

public:
    explicit SideMenu(QWidget *parent);

    int addMode(const QString &name);
    void addSubTab(const QString &name);   // goes under the last added mode

    void toggle();
    void open();
    void closeMenu();

signals:
    // sub is -1 and subName is empty when a main tab was picked
    void tabSelected(int mode, int sub, const QString &subName);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void addButton(const QString &text, bool isSub, int mode, int sub);

    QFrame *panel;            // the part that slides
    QVBoxLayout *list;
    QButtonGroup *group;
    QPropertyAnimation *anim;

    bool opened = false;
    int panelWidth = 240;
    int modeCount = 0;
    int subCount = 0;         // subtabs of the last mode
};

#endif // SIDEMENU_H
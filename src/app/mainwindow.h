#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QMenu;
class QAction;
class QKeySequence;
class QStackedWidget;
class SideMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createMenus();
    void createModeMenu();
    void setTab(int mode, int sub, const QString &subName);
    QAction *addPlaceholder(QMenu *menu, const QString &text,
                            const QKeySequence &shortcut = QKeySequence());

    QStackedWidget *pages;   // one page per mode
    SideMenu *sideMenu;
    QStringList modeNames;
};

#endif // MAINWINDOW_H
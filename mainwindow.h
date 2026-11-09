#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenu;
class QAction;
class QKeySequence;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createMenus();
    QAction *addPlaceholder(QMenu *menu, const QString &text,
                            const QKeySequence &shortcut = QKeySequence());
};

#endif // MAINWINDOW_H
#include "mainwindow.h"

#include <QAction>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Olamon");
    resize(1280, 800);

    createMenus();
    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
}

// builds the top menu bar
void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("File");
    addPlaceholder(fileMenu, "Save", QKeySequence::Save);
    addPlaceholder(fileMenu, "Load", QKeySequence::Open);
    fileMenu->addSeparator();
    addPlaceholder(fileMenu, "Export", QKeySequence("Ctrl+E"));

    QMenu *editMenu = menuBar()->addMenu("Edit");
    addPlaceholder(editMenu, "Undo", QKeySequence::Undo);
    addPlaceholder(editMenu, "Redo", QKeySequence::Redo);
    editMenu->addSeparator();
    addPlaceholder(editMenu, "Copy", QKeySequence::Copy);
    addPlaceholder(editMenu, "Paste", QKeySequence::Paste);

    QMenu *optionsMenu = menuBar()->addMenu("Options");
    addPlaceholder(optionsMenu, "File options");
    addPlaceholder(optionsMenu, "App options");
}

// temporary action that only reports in the status bar, will be replaced as features get built
QAction *MainWindow::addPlaceholder(QMenu *menu, const QString &text, const QKeySequence &shortcut)
{
    QAction *action = menu->addAction(text);
    action->setShortcut(shortcut);
    connect(action, &QAction::triggered, this, [this, text]() {
        statusBar()->showMessage(text + " is not implemented yet", 3000);
    });
    return action;
}
#include "mainwindow.h"
#include "sidemenu.h"
#include "writingtab.h"
#include <QAction>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1280, 800);

    pages = new QStackedWidget(this);
    setCentralWidget(pages);

    createMenus();
    createModeMenu();
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

// hamburger button + the slide out menu, also makes the empty pages
void MainWindow::createModeMenu()
{
    modeNames = {"Writing", "Translation", "Dictionary", "Resources"};

    sideMenu = new SideMenu(pages);
    connect(sideMenu, &SideMenu::tabSelected, this, &MainWindow::setTab);

    for (int i = 0; i < modeNames.size(); ++i) {
        sideMenu->addMode(modeNames[i]);

        // writing gets the real tab, the rest are still empty labels
        if (i == 0) {
            pages->addWidget(new WritingTab);
        } else {
            QLabel *page = new QLabel;
            page->setAlignment(Qt::AlignCenter);
            pages->addWidget(page);
        }

        // test subtabs, remove later
        if (modeNames[i] == "Dictionary") {
            sideMenu->addSubTab("Tab1");
            sideMenu->addSubTab("Tab2");
        }
    }

    QToolButton *menuButton = new QToolButton(this);
    menuButton->setText(QString(QChar(0x2630)));
    menuButton->setAutoRaise(true);
    menuButton->setStyleSheet("QToolButton { font-size: 16px; padding: 2px 8px; }");
    connect(menuButton, &QToolButton::clicked, sideMenu, &SideMenu::toggle);
    menuBar()->setCornerWidget(menuButton, Qt::TopLeftCorner);

    setTab(0, -1, QString());
}

// sub is -1 when a main tab was picked
void MainWindow::setTab(int mode, int sub, const QString &subName)
{
    pages->setCurrentIndex(mode);

    QString name = modeNames[mode];
    if (sub >= 0)
        name += " / " + subName;

    QLabel *page = qobject_cast<QLabel *>(pages->widget(mode));
    if (page)
        page->setText(name + " (empty for now)");
    setWindowTitle("Olamon - " + name);
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
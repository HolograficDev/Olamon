#include "fontmanager.h"
#include "mainwindow.h"
#include "theme.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("Olamon");
    QApplication::setApplicationName("Olamon");

    Theme::applyAmoled();   // before any widgets exist, the tool icons pick up their colors on creation
    FontManager::load();

    MainWindow w;
    w.show();
    return a.exec();
}
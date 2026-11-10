#include "fontmanager.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("Olamon");
    QApplication::setApplicationName("Olamon");

    FontManager::load();

    MainWindow w;
    w.show();
    return a.exec();
}
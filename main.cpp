#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("Olamon");
    QApplication::setApplicationName("Olamon");

    MainWindow w;
    w.show();
    return a.exec();
}
#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

#undef main
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

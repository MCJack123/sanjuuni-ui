#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

extern int sanjuuni_main(int argc, const char * argv[]);

#undef main
int main(int argc, char *argv[]) {
    if (argc > 1) return sanjuuni_main(argc, (const char **)argv);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

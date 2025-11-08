#include <QtWidgets>
#include <QApplication>
#include <QMainWindow>
#include "boardwidget.h"
#include "mainwindow.h"
#include "gomoku.h"
#include "main.moc"
using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}

//#include "moc_main.cpp"

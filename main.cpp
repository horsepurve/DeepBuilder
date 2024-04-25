#include "mainwindow.h"
#include <QApplication>
#include <QFile>

// main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setWindowIcon(QIcon("Icon.ico"));

    QFile styleFile(":/style.css");
    styleFile.open(QFile::ReadOnly);
    a.setStyleSheet(styleFile.readAll());
    styleFile.close();

    MainWindow w;
    w.show();

    return a.exec();
}

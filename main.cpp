#include "mainwindow.h"
#include <QApplication>
#include <locale.h>
int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    QApplication a(argc, argv);
    MainWindow firstWindow("Pinkie Pie"), secondWindow("Twilight Sparkle");
    firstWindow.connectToCOM(0);
    secondWindow.connectToCOM(1);
    firstWindow.show();
    secondWindow.show();
    return a.exec();
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    HANDLE hPortW, hPortR, hThreadArray, events[4];
    bool isConnected;
    int eventNumber;
public:
    explicit MainWindow(QString title = "MainWindow", QWidget *parent = 0);
    void connectToCOM(int eventNum);
    friend DWORD WINAPI threadFunction(LPVOID lpParam);
    int getSpeed();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
public slots:
    void sendCOMMessage();
signals:
    void messageSended();
};

#endif // MAINWINDOW_H

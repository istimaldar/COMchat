#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#define FIRST_COM TEXT("\\\\.\\COM2")
#define SECOND_COM TEXT("\\\\.\\COM3")

typedef struct MyData {
    MainWindow *window;
} MYDATA, *PMYDATA;
DWORD WINAPI threadFunction(LPVOID lpParam);

MainWindow::MainWindow(QString title, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(title);
    isConnected = false;
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(sendCOMMessage()));
}

void MainWindow::connectToCOM(int eventNum)
{

    events[0] = CreateEvent(NULL, FALSE, FALSE, TEXT("SendEvent1"));
    events[1] = CreateEvent(NULL, FALSE, FALSE, TEXT("SendEvent2"));
    events[2] = CreateEvent(NULL, FALSE, FALSE, TEXT("RecivieEvent1"));
    events[3] = CreateEvent(NULL, FALSE, FALSE, TEXT("RecivieEvent2"));
    eventNumber = eventNum;
    DWORD   dwThreadIdArray;
    PMYDATA pData;


    isConnected = true;
    pData = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(MYDATA));
    pData->window=this;
    hThreadArray = CreateThread( // Create new thread
            NULL,                   // default security attributes
            0,                      // use default stack size
            threadFunction,       // thread function name
            pData,          // argument to thread function
            0,                      // use default creation flags
            &dwThreadIdArray);


}

DWORD WINAPI threadFunction(LPVOID lpParam)
{

    PMYDATA pData;
    HANDLE hPortR;
    pData = (PMYDATA)lpParam;
    DCB dcb;
    DWORD       dwBytesTransferred;
    while (true) {
    WaitForSingleObject(pData->window->events[pData->window->eventNumber],INFINITE);
    hPortR = CreateFile(
           pData->window->eventNumber == 0 ? FIRST_COM : SECOND_COM,
           GENERIC_READ | GENERIC_WRITE,
           0,
           NULL,
           OPEN_EXISTING,
           0,
           NULL
    );

    if(!GetCommState(hPortR,&dcb))
        return 1;
    dcb.BaudRate = pData->window->ui->comboBox->currentText().toInt();          //9600 Baud
    dcb.ByteSize = 8;                 //8 data bits
    dcb.Parity = NOPARITY;            //no parity
    if(!SetCommState(hPortR,&dcb))
        return 1;
    int lenght;
    ReadFile(hPortR,(void*) &lenght, sizeof(int), &dwBytesTransferred, 0);  //read 1
    char data[lenght+1];
    data[lenght] = '\0';
    SetEvent(pData->window->events[1-pData->window->eventNumber+2]);
    ReadFile(hPortR,(void*) &data, lenght * sizeof(char), &dwBytesTransferred, 0);  //read 1
    pData->window->ui->listWidget->addItem(QString(data));

    CloseHandle(hPortR);
    SetEvent(pData->window->events[1-pData->window->eventNumber+2]);
}
    return 0;
}

void MainWindow::sendCOMMessage() {
    DCB dcb;
    DWORD byteswritten;
    SetEvent(events[1-eventNumber]);
    hPortW = CreateFile(
           eventNumber == 0 ? FIRST_COM : SECOND_COM,
           GENERIC_READ | GENERIC_WRITE,
           0,
           NULL,
           OPEN_EXISTING,
           0,
           NULL
    );

    if(!GetCommState(hPortW,&dcb))
        return;
    dcb.BaudRate = ui->comboBox->currentText().toInt();          //9600 Baud
    dcb.ByteSize = 8;                 //8 data bits
    dcb.Parity = NOPARITY;            //no parity
    if(!SetCommState(hPortW,&dcb))
        return;
    QString message = this->windowTitle();
    message += ": ";
    message += ui->lineEdit->text();
    ui->lineEdit->clear();
    ui->listWidget->addItem(message + QString(" [You]"));
    int lenght = message.toStdString().length();
    WriteFile(hPortW,&lenght,sizeof(int),&byteswritten,NULL);
    WaitForSingleObject(events[eventNumber+2],INFINITE);
    WriteFile(hPortW,message.toStdString().c_str(),lenght * sizeof(char),&byteswritten,NULL);


    WaitForSingleObject(events[eventNumber+2],INFINITE);
    CloseHandle(hPortW);
}

MainWindow::~MainWindow()
{
    if(eventNumber==0) for(int i = 0;i<4;i++) CloseHandle(events[i]);
    delete ui;
}

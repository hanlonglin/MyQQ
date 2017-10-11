#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QHostAddress>
#include <QProgressBar>


#include "charwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QUdpSocket bindSocket;

    QTcpServer fileServer;
    QTcpSocket *fileSocket;

    //属性
    bool isStart=true;
    QFile file;
    QString fname;
    qint64 fsize=0;
    qint64 recSize=0;



private slots:

    /*连接*/
    void connectSlot();

    /*接收消息*/
    void recMesSlot();

    /*接收文件*/
    void recFileSlot();

    /*开始监听*/
    void startListenSlot();

};

#endif // MAINWINDOW_H

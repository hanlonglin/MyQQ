#ifndef CHARWINDOW_H
#define CHARWINDOW_H

#include <QWidget>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QTimer>

namespace Ui {
class CharWindow;
}

class CharWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CharWindow(QWidget *parent = 0);
    ~CharWindow();

    //设置数据，显示之前调用
    void setData(QString ip,qint64 port);

    //发送文件内容
    void sendFileContent();

private:
    Ui::CharWindow *ui;

    //属性
    QString IP;
    qint64 PORT;

    QString fpath;
    QString fname;
    qint64 fsize;

    //定时
    QTimer timer;
    //发送消息
    QUdpSocket sendSocket;
    //发送文件
    QTcpSocket fileSocket;

private slots:

    /*发送消息  UDP*/
    void sendMesSlot();

    /*发送文件*/
    void sendFileSlot();
};

#endif // CHARWINDOW_H

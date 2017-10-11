#include "charwindow.h"
#include "ui_charwindow.h"

CharWindow::CharWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonSendMes,&QPushButton::clicked,this,&CharWindow::sendMesSlot);//发送消息
    connect(ui->lineEditMes,&QLineEdit::returnPressed,this,&CharWindow::sendMesSlot);//发送消息

    connect(ui->pushButtonSendFile,&QPushButton::clicked,this,&CharWindow::sendFileSlot);//发送文件

//    connect(&timer,&QTimer::timeout,
//            [=]()
//    {
//        timer.stop();
//        sendFileContent();
//    });

    //发送文件内容
    connect(&fileSocket,&QTcpSocket::readyRead,  //确认发送
            [=]()
    {
        QString retMes=fileSocket.readAll();
        if(retMes=="ok")
        {
            sendFileContent();
        }else if(retMes=="cancel")
        {
            fileSocket.disconnectFromHost();
            fileSocket.close();
            ui->textEditContent->append("<span style='color:blue;'>"+QString::fromLocal8Bit("文件被拒绝")+"</span>");
        }
    });



}

CharWindow::~CharWindow()
{
    delete ui;
}

void CharWindow::setData(QString ip, qint64 port)
{
    IP=ip;
    PORT=port;
    setWindowTitle(QString::fromLocal8Bit("正在与%1:%2聊天").arg(ip).arg(port));
}

/*发送消息*/
void CharWindow::sendMesSlot()
{
    QByteArray mes=ui->lineEditMes->text().toUtf8();
    if(mes.isEmpty())
        return ;
    qint64 len=sendSocket.writeDatagram(mes,QHostAddress(IP),PORT);
    if(len>0)
    {
        QString time=QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        ui->textEditContent->append("--"+time+"--me");
        ui->textEditContent->append("<span style='color:blue;'>"+mes+"</span>");
        qDebug()<<"send success";
    }else
    {
        QMessageBox::information(this,"","send error");
    }
}

/*发送文件*/
void CharWindow::sendFileSlot()
{
    //先建立连接
    qDebug()<<"before connect fileSocket state:"<<fileSocket.state();
    fileSocket.connectToHost(QHostAddress(IP),PORT);
    qDebug()<<"after connect fileSocket state:"<<fileSocket.state();

    //获取文件信息
    QString path=QFileDialog::getOpenFileName(this,"","");
    if(path.isEmpty()) return ;
    fpath=path;
    QFileInfo finfo(path);
    fname=finfo.fileName();
    fsize=finfo.size();

    //先发送文件头信息 文件名##文件大小
    QString head=QString("%1##%2").arg(fname).arg(fsize);
    qDebug()<<"head"<<head;
    qint64 len=fileSocket.write(head.toUtf8());

    QString time=QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    ui->textEditContent->append("--"+time+"--me");
    ui->textEditContent->append("<span style='color:blue;'>"+QString::fromLocal8Bit("正在发送文件：")+fname+"......</span>");
//    if(len>0)
//    {

//    }


}

//****************************functions

/*发送文件内容*/
void CharWindow::sendFileContent()
{
    QFile file(fpath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,"",QString::fromLocal8Bit("打开文件失败"));
        return ;
    }
    qint64 len=0;
    qint64 sendSize=0;
    do
    {
        len=0;
        char buf[1024*4]={0};
        len=file.read(buf,sizeof(buf));
        len=fileSocket.write(buf,len);
        sendSize+=len;
    }while(len>0);
    //文件是否发送完毕
    if(sendSize==fsize)
    {
        file.close();
        fileSocket.disconnectFromHost();
        fileSocket.close();
        QMessageBox::information(this,"",QString::fromLocal8Bit("发送文件完毕！"));
        ui->textEditContent->append("<span style='color:blue;'>"+QString::fromLocal8Bit("文件发送完毕")+"</span>");
    }

}

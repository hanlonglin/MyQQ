#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString::fromLocal8Bit("QQ主界面"));

    connect(ui->pushButtonConnect,&QPushButton::clicked,this,&MainWindow::connectSlot);//连接

    //默认监听9999
    //qDebug()<<"before bind state:"<<bindSocket.state();
    bindSocket.bind(QHostAddress::Any,9999);
    connect(&bindSocket,&QUdpSocket::readyRead,this,&MainWindow::recMesSlot);//接收消息
    //qDebug()<<"after bind state:"<<bindSocket.state();
    connect(ui->pushButtonStartListen,&QPushButton::clicked,this,&MainWindow::startListenSlot);//开始监听


    //监听文件
    fileServer.listen(QHostAddress::Any,9999);
    connect(&fileServer,&QTcpServer::newConnection,
            [=]()
    {
        fileSocket=fileServer.nextPendingConnection();
        connect(fileSocket,&QTcpSocket::readyRead,this,&MainWindow::recFileSlot);
        //        QString peerIp=fileSocket->peerAddress().toString();
        //        quint16 peerPort=fileSocket->peerPort();
        //        QMessageBox box;
        //        box.setText(QString::fromLocal8Bit("收到来自%1:%2的文件传输连接，是否同意？").arg(peerIp).arg(peerPort));
        //        box.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);
        //        int ret=box.exec();
        //        qDebug()<<"ret"<<ret;
        //        if(ret==QMessageBox::Ok)
        //        {
        //             connect(fileSocket,&QTcpSocket::readyRead,this,&MainWindow::recFileSlot);
        //        }else
        //        {

        //        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectSlot()
{
    QString ip=ui->lineEditIp->text();
    qint64 port=ui->lineEditPort->text().toInt();
    if(ip.isEmpty()||port==0)
    {
        QMessageBox::information(this,"","ip or port can't be empty!");
        return ;
    }

    //打开窗口
    CharWindow *charWindow=new CharWindow;
    charWindow->setAttribute(Qt::WA_DeleteOnClose);
    charWindow->setData(ip,port);
    charWindow->show();

}

//接收消息
void MainWindow::recMesSlot()
{
    char buf[1024]={0};
    QHostAddress peerIp;
    quint16 peerPort;
    qint64 len=bindSocket.readDatagram(buf,sizeof(buf),&peerIp,&peerPort);
    if(len>0)
    {
        QString str=QString::fromLocal8Bit("收到来自<span style='color:blue'>%1:%2</span>的消息:<span style='color:green'>%3</span>").arg(peerIp.toString()).arg(peerPort).arg(buf);
        ui->textEditRev->append(str);
    }else
    {
        QMessageBox::information(this,"","接收消息失败！");
    }

}
//接收文件
void MainWindow::recFileSlot()
{
    QByteArray buf=fileSocket->readAll();
    if(isStart)
    {
        qDebug()<<"buf"<<QString(buf);
        isStart=false;
        fname=QString(buf).section("##",0,0);
        fsize=QString(buf).section("##",1,1).toInt();
        QString peerIp=fileSocket->peerAddress().toString();
        quint16 peerPort=fileSocket->peerPort();

        qDebug()<<"fsize"<<fsize;
        QString str=QString::fromLocal8Bit("收到来自<span style='color:blue'>%1:%2</span>的文件: <span style='color:red'>%3</span>......:").arg(peerIp).arg(peerPort).arg(fname);
        ui->textEditRev->append(str);

        QMessageBox box;
        box.setText(QString::fromLocal8Bit("收到来自%1:%2的文件%3，是否同意？").arg(peerIp).arg(peerPort).arg(fname));
        box.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok|QMessageBox::Save);
        int ret=box.exec();
        qDebug()<<"ret"<<ret;
        QString savePath=fname;
        if(ret==QMessageBox::Cancel)
        {
            QString str=QString::fromLocal8Bit("文件已取消接收");
            ui->textEditRev->append(str);
            fileSocket->write("cancel");
            recSize=0;
            isStart=true;
            //disconnect(fileSocket,&QTcpSocket::readyRead,this,&MainWindow::recFileSlot);
            return;
        }else if(ret==QMessageBox::Save)
        {
            QString saveAsDir=QFileDialog::getExistingDirectory(this,QString::fromLocal8Bit("另存为"),"");
            savePath=saveAsDir+"/"+fname;
        }


        //打开文件
        file.setFileName(savePath);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this,"",QString::fromLocal8Bit("打开文件失败"));
            return ;
        }
        fileSocket->write("ok");

    }else
    {
        qint64 len =file.write(buf);
        recSize+=len;
        qDebug()<<"recSize"<<recSize;
        if(recSize==fsize)
        {
            qDebug()<<"finish recv";
            file.close();
            fileSocket->disconnectFromHost();
            fileSocket->close();
            QMessageBox::information(this,"",QString::fromLocal8Bit("文件接收完成！"));
            QString str=QString::fromLocal8Bit("文件接收完毕.");
            ui->textEditRev->append(str);
            isStart=true;
            recSize=0;
        }

    }
}

/*开始监听*/
void MainWindow::startListenSlot()
{
    qint64 port=ui->lineEditListenPort->text().toInt();
    if(port==0)
        return ;
    bindSocket.close();
    bool ret=bindSocket.bind(QHostAddress::Any,port);
    fileServer.close();
    ret=fileServer.listen(QHostAddress::Any,port);
    if(!ret)
    {
        QMessageBox::information(this,"","bind fail");
        return ;
    }
    ui->labelListenPort->setText(QString::number(port));
}

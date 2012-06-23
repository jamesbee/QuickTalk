#include <QMessageBox>
#include <QDateTime>
#include "quicktalk.h"
#include "ui_quicktalk.h"

QuickTalk::QuickTalk(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickTalk)
{
    ui->setupUi(this);
    //初始化
    __address.setAddress(setIt.value("host-address").toString());
    __port = setIt.value("host-port").toString();
    //新建的快捷键
    ctrlEnter = new QShortcut(Qt::CTRL+Qt::Key_Return,this);
    helpMsg = new QShortcut(Qt::Key_F1,this);
    quit = new QShortcut(Qt::CTRL+Qt::Key_Q,this);
    //独立初始化方法
    initializ();
}

QuickTalk::~QuickTalk()
{
    udpSocket.close();
    collectSocket.close();
    delete ui;
}

//初始化
void QuickTalk::initializ()
{
    //将端口的范围限定
    QLineEdit *edit;
    edit = ui->lineEditProt;
    edit->setValidator(new QIntValidator(0,65535,edit));

    //检查是否保留有上次操作地址参数
    //默认端口为6789
    if(0 == __port.length())
        __port = "6789";
    if(0 == __address.toString().length())
        this->alertNullAdress();
    else
    {
        this->alertNullAdress(__address.toString());
        this->collectSocket.bind(__port.toUShort(),QUdpSocket::ShareAddress);
    }

    //####################
    //#    连接信号与槽
    //####################
    //发送
    connect(ui->sentButton,SIGNAL(clicked()),
            this,SLOT(sendMessage()));
    connect(ctrlEnter,SIGNAL(activated()),
            this,SLOT(sendMessage()));
    //退出
    connect(ui->closeButton,SIGNAL(clicked()),
            this,SLOT(reject()));
    connect(quit,SIGNAL(activated()),
            this,SLOT(reject()));
    //设置主机地址
    connect(ui->setAddressButton,SIGNAL(clicked()),
            this,SLOT(setupAddress()));
    //收到信息
    connect(&collectSocket,SIGNAL(readyRead()),
            this,SLOT(processPendingMessage()));
    //显示帮助信息
    connect(helpMsg,SIGNAL(activated()),
            this,SLOT(showHelpMsg()));
}

//信号：收到信息事产生特效


//槽：设置主机地址【设置按钮按下&初始化】
void QuickTalk::setupAddress()
{
    __address = ui->lineEditHost->text();
    __port = ui->lineEditProt->text();
    if(0 == __port.length())
        __port = "6789";
    if(0 == __address.toString().length())
        this->alertNullAdress();
    else
    {
        this->alertNullAdress(__address.toString() + ":" + __port);
        setIt.setValue("host-address",__address.toString());
        setIt.setValue("host-port",__port);
        this->collectSocket.bind(__port.toUShort(),QUdpSocket::ShareAddress);
    }
}

//槽：发送信息【按钮按下&ctrl+回车】
void QuickTalk::sendMessage()
{
    QString _outMessage = ui->textEdit->toPlainText();
    //提示地址空
    if(0 == __address.toString().length())
        this->alertNullAdress();
    //若信息不为空
    else if(0 != _outMessage.length())
    {
        //将小于号替换为&lt
        //_outMessage.replace("<","&lt");

        ui->textEdit->setText("");
        QByteArray datagram;
        QDataStream out(&datagram,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out<<QDateTime::currentDateTime()<<_outMessage;
        udpSocket.writeDatagram(datagram,__address,__port.toUShort());

        //插入发送信息
        QString _msgToPend;
        _msgToPend = QString("<font color=gray>%1</font><font color=red> 我说>> </font>").
                arg(QDateTime::currentDateTime().toString());
        ui->textEditMessage->append(_msgToPend);
        ui->textEditMessage->append(_outMessage);

        //Debug only
        //QString _debugMsg = "发送信息到：" + __address.toString() + ":" + __port;
        //qDebug()<<_debugMsg;
    }
}

//槽：收到信息【从socket收到信息】
void QuickTalk::processPendingMessage()
{
    QByteArray datagram;
    do{
        datagram.resize(collectSocket.pendingDatagramSize());
        collectSocket.readDatagram(datagram.data(),datagram.size());
    }while(collectSocket.hasPendingDatagrams());

    QDateTime time;
    QString _inMessage;

    QDataStream in(&datagram,QIODevice::ReadOnly);
    in>>time>>_inMessage;

    //插入新信息
    QString _msgToPend;
    //_inMessage.replace("&lt","<");
    _msgToPend = QString("<font color=gray>%1</font><font color=green> 收到@@ </font>").
            arg(QDateTime::currentDateTime().toString());
    ui->textEditMessage->append(_msgToPend);
    ui->textEditMessage->append(_inMessage);
}

//槽：帮助信息【按下F1】
void QuickTalk::showHelpMsg()
{
    QMessageBox::about(this,tr("帮助"),
                           tr("<h2>局域网短信</h2>"
                              "<p>1. 快捷键一览：ctrl+q 退出；ctrl+enter 发送。</p>"
                              "<p>2.别的没什么了，功能就这么多</p>"
                              "<p>╮(╯_╰)╭"));
}

//槽：继承覆盖Dialog的reject方法，以屏蔽Esc键
void QuickTalk::reject()
{
    if(QMessageBox::Ok == QMessageBox::information(this,tr("退出"),
                                tr("程序将会退出，终结会话？"),
                                QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel))
    {
        this->done(1313);
    }
    return;
}

//提示主机地址未设置
void QuickTalk::alertNullAdress(const QString &address)
{
    if("" == address)
        QMessageBox::information(this,tr("地址为空"),tr("地址为空，设置主机地址。\n"),QMessageBox::Ok);
    else
        QMessageBox::information(this,tr("主机地址"),tr("设置主机地址为：\n") + address + tr("\n"),QMessageBox::Ok);
}

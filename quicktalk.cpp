#include <QMessageBox>
#include <QDateTime>
#include "quicktalk.h"
#include "ui_quicktalk.h"

QuickTalk::QuickTalk(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickTalk)
{
    ui->setupUi(this);
    //��ʼ��
    __address.setAddress(setIt.value("host-address").toString());
    __port = setIt.value("host-port").toString();
    //�½��Ŀ�ݼ�
    ctrlEnter = new QShortcut(Qt::CTRL+Qt::Key_Return,this);
    helpMsg = new QShortcut(Qt::Key_F1,this);
    quit = new QShortcut(Qt::CTRL+Qt::Key_Q,this);
    //������ʼ������
    initializ();
}

QuickTalk::~QuickTalk()
{
    udpSocket.close();
    collectSocket.close();
    delete ui;
}

//��ʼ��
void QuickTalk::initializ()
{
    //���˿ڵķ�Χ�޶�
    QLineEdit *edit;
    edit = ui->lineEditProt;
    edit->setValidator(new QIntValidator(0,65535,edit));

    //����Ƿ������ϴβ�����ַ����
    //Ĭ�϶˿�Ϊ6789
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
    //#    �����ź����
    //####################
    //����
    connect(ui->sentButton,SIGNAL(clicked()),
            this,SLOT(sendMessage()));
    connect(ctrlEnter,SIGNAL(activated()),
            this,SLOT(sendMessage()));
    //�˳�
    connect(ui->closeButton,SIGNAL(clicked()),
            this,SLOT(reject()));
    connect(quit,SIGNAL(activated()),
            this,SLOT(reject()));
    //����������ַ
    connect(ui->setAddressButton,SIGNAL(clicked()),
            this,SLOT(setupAddress()));
    //�յ���Ϣ
    connect(&collectSocket,SIGNAL(readyRead()),
            this,SLOT(processPendingMessage()));
    //��ʾ������Ϣ
    connect(helpMsg,SIGNAL(activated()),
            this,SLOT(showHelpMsg()));
}

//�źţ��յ���Ϣ�²�����Ч


//�ۣ�����������ַ�����ð�ť����&��ʼ����
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

//�ۣ�������Ϣ����ť����&ctrl+�س���
void QuickTalk::sendMessage()
{
    QString _outMessage = ui->textEdit->toPlainText();
    //��ʾ��ַ��
    if(0 == __address.toString().length())
        this->alertNullAdress();
    //����Ϣ��Ϊ��
    else if(0 != _outMessage.length())
    {
        //��С�ں��滻Ϊ&lt
        //_outMessage.replace("<","&lt");

        ui->textEdit->setText("");
        QByteArray datagram;
        QDataStream out(&datagram,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out<<QDateTime::currentDateTime()<<_outMessage;
        udpSocket.writeDatagram(datagram,__address,__port.toUShort());

        //���뷢����Ϣ
        QString _msgToPend;
        _msgToPend = QString("<font color=gray>%1</font><font color=red> ��˵>> </font>").
                arg(QDateTime::currentDateTime().toString());
        ui->textEditMessage->append(_msgToPend);
        ui->textEditMessage->append(_outMessage);

        //Debug only
        //QString _debugMsg = "������Ϣ����" + __address.toString() + ":" + __port;
        //qDebug()<<_debugMsg;
    }
}

//�ۣ��յ���Ϣ����socket�յ���Ϣ��
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

    //��������Ϣ
    QString _msgToPend;
    //_inMessage.replace("&lt","<");
    _msgToPend = QString("<font color=gray>%1</font><font color=green> �յ�@@ </font>").
            arg(QDateTime::currentDateTime().toString());
    ui->textEditMessage->append(_msgToPend);
    ui->textEditMessage->append(_inMessage);
}

//�ۣ�������Ϣ������F1��
void QuickTalk::showHelpMsg()
{
    QMessageBox::about(this,tr("����"),
                           tr("<h2>����������</h2>"
                              "<p>1. ��ݼ�һ����ctrl+q �˳���ctrl+enter ���͡�</p>"
                              "<p>2.���ûʲô�ˣ����ܾ���ô��</p>"
                              "<p>�r(�s_�t)�q"));
}

//�ۣ��̳и���Dialog��reject������������Esc��
void QuickTalk::reject()
{
    if(QMessageBox::Ok == QMessageBox::information(this,tr("�˳�"),
                                tr("���򽫻��˳����ս�Ự��"),
                                QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel))
    {
        this->done(1313);
    }
    return;
}

//��ʾ������ַδ����
void QuickTalk::alertNullAdress(const QString &address)
{
    if("" == address)
        QMessageBox::information(this,tr("��ַΪ��"),tr("��ַΪ�գ�����������ַ��\n"),QMessageBox::Ok);
    else
        QMessageBox::information(this,tr("������ַ"),tr("����������ַΪ��\n") + address + tr("\n"),QMessageBox::Ok);
}

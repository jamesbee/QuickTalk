#ifndef QUICKTALK_H
#define QUICKTALK_H

#include <QDialog>
#include <QShortcut>
#include <QSettings>
#include <QtNetwork/QUdpSocket>
namespace Ui
{
    class QuickTalk;
}

class QuickTalk : public QDialog
{
    Q_OBJECT
public:
    explicit QuickTalk(QWidget *parent = 0);
    ~QuickTalk();
    
signals:
    //void getAMessage();
    
public slots:
    void setupAddress();
    void alertNullAdress(const QString& address = QString());
    void sendMessage();
    void processPendingMessage();
    void showHelpMsg();
    void reject();
    
private:
    Ui::QuickTalk *ui;

    QHostAddress __address;
    QString __port;

    QUdpSocket udpSocket;
    QUdpSocket collectSocket;

    QSettings setIt;

    QShortcut *ctrlEnter;
    QShortcut *helpMsg;
    QShortcut *quit;

    void initializ();
};

#endif // QUICKTALK_H

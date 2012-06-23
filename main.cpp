#include <QtGui/QApplication>
#include <QSettings>
#include <QTextCodec>
#include <quicktalk.h>

#ifndef ORG_NAME
#define ORG_NAME "SCU&FY-JAM"
#endif
#ifndef APP_NAME
#define APP_NAME "QuickTalk"
#endif

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationName(ORG_NAME);
    QCoreApplication::setApplicationName(APP_NAME);

    QApplication app(argc,argv);

    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);

    QuickTalk qT;
    qT.show();

    return app.exec();
}

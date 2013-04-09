#include "myclass.h"


MyClass::MyClass()
{
    m_manager = new QNetworkAccessManager(this);

    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
         this, SLOT(replyFinished(QNetworkReply*)));

}

void MyClass::fetch(QString addr)
{
    m_manager->get(QNetworkRequest(QUrl(addr)));

    eloop.exec();

}

void MyClass::replyFinished(QNetworkReply* pReply)
{

    QByteArray data=pReply->readAll();
    QString str(data);
    result = str;

    eloop.exit();
    //process str any way you like!

}

#ifndef MYCLASS_H
#define MYCLASS_H
#include <QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QString>
#include <QtNetwork/QNetworkReply>
#include <QDebug>
#include <QObject>

class MyClass : public QObject
{
Q_OBJECT

public:
    MyClass();
    void fetch(QString addr);
    QString result;
    QEventLoop eloop;

public slots:
    void replyFinished(QNetworkReply*);

private:
    QNetworkAccessManager* m_manager;
};


#endif // MYCLASS_H

#include <QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QString>
#include <QStringList>
#include <QtNetwork/QNetworkReply>
#include <QDebug>
#include "myclass.h"




int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    //return a.exec();

    QStringList as = a.arguments();
    as.removeFirst();
    QString query = as.join("+and+");

    MyClass my;
    my.fetch("http://www.ams.org/mathscinet/search/publications.html?pg7=ALLF&s7="+query+"&Submit=Search&fmt=bibtex");

    QStringList lst = my.result.split("<pre>");

    lst.removeFirst();

    QStringList res;

    foreach(QString str, lst)
    {
        QStringList lst2 = str.split("</pre>");

        if (lst2.count() > 1)
            res.append(lst2.first());
    }

    qDebug("%s", res.join("\r\n").toStdString().c_str());
}

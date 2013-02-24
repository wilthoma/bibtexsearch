#ifndef MYTREEPROXYFILTER_H
#define MYTREEPROXYFILTER_H

#include <QSortFilterProxyModel>

class MyTreeProxyFilter : public QSortFilterProxyModel
{
public:
    MyTreeProxyFilter(QObject *parent = NULL);
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
private :
    bool hasToBeDisplayed(const QModelIndex index) const;
};

#endif // MYTREEPROXYFILTER_H

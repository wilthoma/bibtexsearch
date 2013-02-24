#include "mytreeproxyfilter.h"

#include <QtDebug>
#include <QStringList>
#include "treeitem.h"

MyTreeProxyFilter::MyTreeProxyFilter(QObject *parent):
        QSortFilterProxyModel(parent)
{
}

bool MyTreeProxyFilter::filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const
 {
     QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

     return hasToBeDisplayed(index);
 }

bool MyTreeProxyFilter::hasToBeDisplayed(const QModelIndex index) const
{
    bool result = false;
    // How many child this element have
    if ( sourceModel()->rowCount(index) > 0 )
    {
        for( int ii = 0; ii < sourceModel()->rowCount(index); ii++)
        {
            QModelIndex childIndex = sourceModel()->index(ii,0,index);
            if ( ! childIndex.isValid() )
                break;
            result = hasToBeDisplayed(childIndex);
            if (result)
            {
                // there is atless one element to display
                break;
            }
        }
    }
    else
    {
        QModelIndex useIndex = sourceModel()->index(index.row(), 1, index.parent());
        TreeItem *item = static_cast<TreeItem*>(useIndex.internalPointer());
        if (item != 0)
        {
            //QString type = sourceModel()->data(useIndex, Qt::DisplayRole).toString();
            QString type = QString::fromUtf8(item->entry.SearchableText.c_str());

            //qDebug() << filterRegExp().pattern();
            QStringList filters = filterRegExp().pattern().split(' ', QString::SkipEmptyParts);

            result = true;
            foreach (QString s, filters)
            {
                if (!type.contains(s, Qt::CaseInsensitive))
                {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/

#include <QtGui>

#include "treeitem.h"
#include "treemodel.h"
#include "bibentry.h"
#include "mainwindow.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/fusion/adapted/std_tuple.hpp>
#include <iostream>
#include<fstream>
#include<iterator>
#include <string>
#include <complex>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace bsta = boost::algorithm;

typedef std::string mystring;
typedef boost::fusion::pair<mystring, mystring> kvpair;
typedef std::map<std::string, std::string> kvpairs;
//typedef boost::fusion::map<mystring, mystring> kvpairs;


//namespace client{


bibentry fileentry;
bibentry errorentry;

TreeModel::TreeModel(const QStringList &bibFiles, QObject *parent )
    : QAbstractItemModel(parent)
{
    fileentry.type = 1;
    errorentry.type = 2;
    QList<QVariant> rootData;
    rootData << "Title" << "Authors";
    rootItem = new TreeItem(rootData, fileentry);
    setupModelData(bibFiles, rootItem);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if (role == Qt::DecorationRole && index.column()==0)
    {
        //return QIcon(":/icons/img/Page.png");
        if (item)
        {
            switch (item->entry.type)
            {
            case 0:
                return QIcon(":/icons/img/Page.png");
            case 1:
                return QPixmap(":/icons/img/book_notebook.png");
            case 2:
                return QPixmap(":/icons/img/WarningHS.png");
            }
        }
    }

    if (role != Qt::DisplayRole)
        return QVariant();



    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}



BOOST_FUSION_ADAPT_STRUCT(
    bibentry,
    (mystring, entrytype)
    (mystring, entrykey)
    (kvpairs, attribus)
)

//typedef std::map<std::string, std::string> rettype;
typedef bibentry rettype;

template <typename Iterator>
struct keys_and_values
  : qi::grammar<Iterator, rettype(), qi::space_type>
{
    keys_and_values()
      : keys_and_values::base_type(start)
    {
        using qi::lexeme;
        //using ascii::char_;

        quoted_string = lexeme['"' >> *(qi::char_ - '"') >> '"'];
        braced_string = '{' >> *(braced_string | wordinbrace) >> '}';
        wordinbrace = lexeme[ +(qi::char_ - '}'-'{') ];

        //query =  typekeypair >> pair >> *((qi::lit(';') | ',') >> pair) >> '}'; // '@' >> key >>
        start = '@' >> key >> '{' >> key >> ',' >> query;
        query =  pair >> *((qi::lit(';') | ',') >> pair) >> (-(qi::lit(',') | '.')) >> '}'; // '@' >> key >>
        pair  =  key >> -('=' >> value);
        typekeypair = '@' >> key >> '{' >> key >> ',';
        key   =  qi::char_("a-zA-Z_") >> *(qi::char_ - "," - "{" - "}" - "=" - "\"");
        value = quoted_string | braced_string | plainvalue ;//| (+qi::char_("a-zA-Z_0-9"));
        plainvalue = +qi::char_("a-zA-Z_0-9");
    }
    qi::rule<Iterator, std::string()> quoted_string;
    qi::rule<Iterator, rettype(), qi::space_type> start;
    qi::rule<Iterator, std::map<std::string, std::string>(), qi::space_type> query;
    qi::rule<Iterator, std::pair<std::string, std::string>(), qi::space_type> pair;
    qi::rule<Iterator, std::pair<std::string, std::string>(), qi::space_type> typekeypair;
    qi::rule<Iterator, std::string(), qi::space_type> key, value, plainvalue, braced_string, wordinbrace;
};
/**/

//}
/*
BOOST_FUSION_ADAPT_STRUCT(
    bibentry,
    (std::string, entrytype)
    (kvpairs, attribus)
)

template <typename Iterator>
    struct bibentry_parser : qi::grammar<Iterator, bibentry(), ascii::space_type>
    {
        bibentry_parser() : bibentry_parser::base_type(start)
        {
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using qi::lexeme;
            using ascii::char_;

            quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];
            identifier %= lexeme[ +(char_ - '{' - "=")];
            keyvaluepair = identifier >> '=' >> quoted_string;
            keyvaluepairs = keyvaluepair >> *((qi::lit(',') | ';') >> keyvaluepair);

            start %=
                lit("@")
                >> identifier
                >> '{'
                >> keyvaluepairs
                //>>  keys_and_values<Iterator>()
                >>  '}'
                ;
        }

        qi::rule<Iterator, mystring(), ascii::space_type> quoted_string;
        qi::rule<Iterator, mystring(), ascii::space_type> identifier;
        qi::rule<Iterator, kvpair(), ascii::space_type> keyvaluepair;
        qi::rule<Iterator, boost::fusion::map<mystring, mystring>(), ascii::space_type> keyvaluepairs;
        qi::rule<Iterator, bibentry(), ascii::space_type> start;
    };
*/

namespace std
{
    template<typename S, typename T>
    std::ostream& operator<< (std::ostream& os, std::map<S,T>& z)
    {
        // do your stuff here
        for(std::map<S,T>::iterator iter = z.begin(); iter != z.end(); ++iter)
        {
            S k =  iter->first;
            T v = iter->second;
            os << k << " : " << v <<std::endl;
            //ignore value
            //Value v = iter->second;
        }
        return os;
    }

    std::ostream& operator<< (std::ostream& os, bibentry& z)
    {
        os << "Type:" << z.entrytype << endl << "Key: " << z.entrykey <<std::endl << "Attributes:" << endl ;

        os << z.attribus;
        os << "Position: " << z.startpos << " to " << z.endpos << endl;


        return os;
    }
}

inline int countLines(const std::string& s, int maxpos)
{
    int cnt = 0;
    for (int i=0;i>maxpos;i++)
        if (s[i] == '\n')
            ++cnt;
    return cnt;
}

bool parseFile(const QString& cFile, std::vector<bibentry>& lst)
{
    //qDebug() << "Parsing file: "<< cFile;
    std::string ccFile = cFile.toUtf8().constData();
    std::ifstream t(ccFile);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    //std::cout << str;

    typedef std::string::const_iterator iterator_type;
    //typedef bibentry_parser<iterator_type> bibentry_parser;
    typedef keys_and_values<iterator_type> bibentry_parser;
    bibentry_parser g;
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    while (iter != end)
    {
        rettype res;
        res.type = 0;
        res.BibFile = cFile.toUtf8().constData();
        res.startpos = std::distance(str.cbegin(), iter);
        bool r = phrase_parse(iter, end, g, qi::space, res);

            if (r )//&& iter == end)
            {
                res.endpos = std::distance(str.cbegin(), iter);
                res.lineNr = countLines(str, res.startpos);
                res.FullText = str.substr(res.startpos, res.endpos-res.startpos-1);
                res.SearchableText.append(res.entrykey);
                res.SearchableText.append("\n");
                for(std::map<std::string,std::string>::iterator miter = res.attribus.begin(); miter != res.attribus.end(); ++miter)
                {
                    res.SearchableText.append(miter->second);
                    res.SearchableText.append("\n");
                }

                //std::cout << "-------------------------\n";
                //std::cout << "Parsing succeeded\n";
                //std::cout << "got: " << boost::fusion::as_vector(res) << std::endl;
                //std::cout << res;
                //std::cout << "\n-------------------------\n";
                //qDebug() <<"Succeeded " << QString::fromUtf8(res.entrykey.c_str());
                lst.push_back( res );
            }
            else
            {
                qDebug() << "-------------------------\n";
                qDebug() << "Parsing failed\n";
                qDebug() << "-------------------------\n";
                return false;
            }
    }
    return true;
}



// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

QString getBibKey( rettype & bi, const QString key )
{
    mystring k2 = key.toUtf8().constData() ;
    k2=trim(k2);
    std::transform(k2.begin(), k2.end(), k2.begin(), ::tolower);

    for(std::map<mystring,mystring>::iterator iter = bi.attribus.begin(); iter != bi.attribus.end(); ++iter)
    {
        mystring k = iter->first ;
        k = trim(k);
        mystring v = iter->second;
        v=trim(v);

        std::transform(k.begin(), k.end(), k.begin(), ::tolower);

        //std::cout << "***" << k << "+++" << v << "+++" << k2 << std::endl;

        if (k.compare(k2) == 0)
        {
            return QString::fromUtf8(v.c_str());
        }
        //ignore value
        //Value v = iter->second;
    }
    return "";
}

void TreeModel::setupModelData(const QStringList &bibFiles, TreeItem *parent)
{

    for( QStringList::ConstIterator entry=bibFiles.begin(); entry!=bibFiles.end(); ++entry )
    {
        if (MainWindow::cancelScanFlag)
            return;

        //std::cout << *entry << std::endl;
        QString bibname = *entry;
        if(bibname != tr(".") && bibname != tr(".."))
        {
            QList<QVariant> ColumnData;
            QFileInfo finfo(bibname);
            ColumnData << finfo.fileName() << "";// finfo.absolutePath();
            bibentry fentry = fileentry;
            fentry.BibFile = bibname.toUtf8().constData();
            TreeItem* bibTreeItem = new TreeItem(ColumnData, fentry, parent);
            parent->appendChild(bibTreeItem);

            std::vector<bibentry> lst;
            if (parseFile(bibname, lst))
            {
                for (std::vector<bibentry>::const_iterator it = lst.begin(); it != lst.end(); ++it)
                {
                    bibentry bib = *it;
                    QList<QVariant> chColumnData;
                    chColumnData << getBibKey(bib, "title") << getBibKey(bib, "author");
                    bibTreeItem->appendChild(new TreeItem(chColumnData, bib, bibTreeItem));
                }

            }
            else
            {
                QList<QVariant> chColumnData;
                chColumnData << "Could not parse file";
                bibTreeItem->appendChild(new TreeItem(chColumnData, errorentry, bibTreeItem));
            }

        }
    }
   /* QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].mid(position, 1) != " ")
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }

        number++;
    }
    */
}

#-------------------------------------------------
#
# Project created by QtCreator 2013-02-16T13:30:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BibTexSearch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    treemodel.cpp \
    treeitem.cpp \
    mytreeproxyfilter.cpp \
    bibentry.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    treemodel.h \
    treeitem.h \
    mytreeproxyfilter.h \
    bibentry.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

INCLUDEPATH += F:/boost/include/boost-1_53
INCLUDEPATH += /home/tw/boost/boost_1_53_0

OTHER_FILES += \
    img/Page.png \
    img/Notepad_32x32.png \
    img/book_notebook.png \
    img/WarningHS.png \
    img/bts.ico \
    license.txt

RESOURCES += \
    icons.qrc

win32:RC_FILE += bts.rc

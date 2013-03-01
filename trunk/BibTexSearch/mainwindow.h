#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include "treemodel.h"
#include "mytreeproxyfilter.h"

namespace Ui {
class MainWindow;
}

enum DetailViewType { DT_TEXT, DT_BIBTEX, DT_BIBITEM };

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QAction *CopyEntryAct;
    QAction *OpenExternallyAct;
    QAction *SettingsAct;
    QAction *DetailTextAct;
    QAction *DetailBibtexAct;
    QAction *DetailBibitemAct;
    QActionGroup *DetailActGroup;
    QAction *ExpandAllAct;
    QAction *CollapseAllAct;
    
    void closeEvent(QCloseEvent *event);

    static QString textEditorCmdLine;
    static QString BibitemFormat;
    static bool cancelScanFlag;

public slots:
    void OnChooseFile();
    void OnTrvClick(const QModelIndex & index);
    void OnTrvDblClick(const QModelIndex & index);
    void OnTrvContextMenuRequested(const QPoint& );
    void copyEntry();
    void showSettings();
    void openExternally();
    void OnFilterChanged(const QString&);
    void OnSelectionChanged(const QItemSelection&, const QItemSelection&);
    void detailTypeText();
    void detailTypeBibtex();
    void detailTypeBibitem();
    void OnAnchorClicked(const QUrl&);
    void cancelScan();
    void expandAll();
    void collapseAll();

private slots:

    void on_cmbFolder_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    void FillTree();
    void AppendAllBibFiles(QString , QStringList&,  QStringList&);
    TreeModel* model;
    MyTreeProxyFilter *theFilter;
    DetailViewType detailViewType;
    void setFolder(const QString& folder);

    QString currentFolder;

    void refreshDetail();
    void loadSettings();
    void saveSettings();
    void doOpenExternally(const QString& bibfile, const int linenr );
};


#endif // MAINWINDOW_H

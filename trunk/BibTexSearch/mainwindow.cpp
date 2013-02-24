#include "mainwindow.h"
#include "treemodel.h"
#include "treeitem.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <QClipboard>
#include <iostream>
#include <QActionGroup>
#include <QSettings>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    detailViewType( DT_TEXT )//,
    //model("hallo\n welt\n welt\nbla\n bli", 0)
{
    ui->setupUi(this);
    //ui->txtFolder->setText("C:\\Users\\alw\\Documents\\Thomas2\\wiltex\\carlo");
    ui->txtFolder->setText("C:\\Users\\alw\\Documents\\Thomas2\\wiltex\\grgrt");
    //ui->txtFolder->setText("C:\\Users\\alw\\Documents\\Thomas2\\wiltex");

    CopyEntryAct = new QAction(tr("&Copy entry"), this);
    CopyEntryAct->setShortcuts(QKeySequence::Copy);
    CopyEntryAct->setStatusTip(tr("Copies this entry to clipboard"));
    QFont f;
    f.setBold(true);
    CopyEntryAct->setFont(f);

    OpenExternallyAct = new QAction(tr("&Open in editor"), this);
    OpenExternallyAct->setShortcuts(QKeySequence::Open);
    OpenExternallyAct->setStatusTip(tr("Opens the .bib file in an editor you can configure"));
    //CopyEntryAct->set

    SettingsAct = new QAction(tr("&Settings"), this);

    DetailActGroup = new QActionGroup(this);

    DetailBibitemAct = new QAction(tr("Bibitem"), this);
    DetailBibitemAct->setCheckable(true);
    DetailBibitemAct->setActionGroup(DetailActGroup);
    DetailBibtexAct = new QAction(tr("Bibtex"), this);
    DetailBibtexAct->setCheckable(true);
    DetailBibtexAct->setActionGroup(DetailActGroup);
    DetailTextAct = new QAction(tr("Formatted Text"), this);
    DetailTextAct->setCheckable(true);
    DetailTextAct->setActionGroup(DetailActGroup);
    DetailTextAct->setChecked(true);

    //SettingsAct->setShortcuts(QKeySequence::Copy);
    //CopyEntryAct->setStatusTip(tr("Copies this entry to clipboard"));

    connect(ui->cmdChooseFolder, SIGNAL(clicked()), this, SLOT(OnChooseFile()));
    connect(ui->trvFiles, SIGNAL(clicked(QModelIndex)), this, SLOT(OnTrvClick(QModelIndex)));
    connect(ui->trvFiles, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnTrvDblClick(QModelIndex)));
    connect(ui->trvFiles, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnTrvContextMenuRequested(QPoint)));
    connect(CopyEntryAct,SIGNAL(triggered()), this, SLOT(copyEntry()) );
    connect(SettingsAct,SIGNAL(triggered()), this, SLOT(showSettings()) );
    connect(OpenExternallyAct,SIGNAL(triggered()), this, SLOT(openExternally()) );
    connect(ui->txtFilter, SIGNAL(textChanged(QString)), this, SLOT(OnFilterChanged(QString)));
    connect(DetailBibitemAct,SIGNAL(triggered()), this, SLOT(detailTypeBibitem()) );
    connect(DetailBibtexAct,SIGNAL(triggered()), this, SLOT(detailTypeBibtex()) );
    connect(DetailTextAct,SIGNAL(triggered()), this, SLOT(detailTypeText()) );


    FillTree();

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    qDebug() << "closeevent";
}

void MainWindow::OnChooseFile()
{
    QString s = QFileDialog::getExistingDirectory(this, "Choose Directory", ui->txtFolder->text());
    if (!s.isEmpty() && !s.isNull())
    {
        ui->txtFolder->setText( s );
        FillTree();
    }
    //return 0;
}

void MainWindow::OnFilterChanged(const QString& txt)
{

    theFilter->setFilterFixedString(txt);
    //theFilter->setFilterKeyColumn(1);
    theFilter->invalidate();
    if (!txt.isEmpty())
        ui->trvFiles->expandAll();
}

void MainWindow::OnSelectionChanged(const QItemSelection& sel1, const QItemSelection& sel2)
{
    refreshDetail();
}

void MainWindow::OnTrvClick(const QModelIndex & index)
{

}
void MainWindow::OnTrvDblClick(const QModelIndex & index)
{
    CopyEntryAct->trigger();
}

void MainWindow::loadSettings()
{
    QSettings s("BibTeXSearch", "BibTeXSearch");
    restoreGeometry(s.value("MainWindowGeometry").toByteArray());
    restoreState(s.value("MainWindowState").toByteArray());
    ui->trvFiles->setColumnWidth(0,s.value("Column0Width", 400).toInt());
    ui->trvFiles->setColumnWidth(1,s.value("Column1Width", 400).toInt());
    textEditorCmdLine = s.value("ExternalEditor", "").toString();
    BibitemFormat = s.value("BibitemFormat", "\\bibitem %author, %title, \\emph{%journal} %volume, %number, %year, %note").toString();
}

void MainWindow::saveSettings()
{
    QSettings s("BibTeXSearch", "BibTeXSearch");
    s.setValue("MainWindowGeometry", saveGeometry());
    s.setValue("MainWindowState", saveState());
    s.setValue("Column0Width", ui->trvFiles->columnWidth(0));
    s.setValue("Column1Width", ui->trvFiles->columnWidth(1));
    s.setValue("ExternalEditor", textEditorCmdLine);
    s.setValue("BibitemFormat", BibitemFormat);
}

inline void appendEntryAttributeHtml(std::string& text, const bibentry& b, const std::string& key )
{
    std::string val = b.getAttribute(key);
    if (!val.empty())
    {
        text.append("<tr><td><b>");
        text.append(key);
        text.append(": </b></td><td>");
        text.append(val);
        text.append("</td></tr>");
    }
}

void MainWindow::refreshDetail()
{
    std::string text("");
    if (detailViewType == DT_TEXT)
    {
        text.append("<html><table>");
    }

    QModelIndexList lst = ui->trvFiles->selectionModel()->selectedIndexes();
    ///std::cout << "+++";
    foreach (QModelIndex ind, lst)
    {
    //QModelIndex ind = ui->trvFiles->selectionModel()->currentIndex()
        if (ind.column() ==0)
        {
            QModelIndex sind = theFilter->mapToSource(ind);
             //qDebug() << "**";
            TreeItem *item = static_cast<TreeItem*>(sind.internalPointer());
            if (item)
            {
                //qDebug() << QString::fromUtf8(item->entry.entrykey.c_str());
                QString bibitemtpl = BibitemFormat;
                switch (detailViewType)
                {
                case DT_TEXT:
                    appendEntryAttributeHtml(text, item->entry, "Title");
                    appendEntryAttributeHtml(text, item->entry, "Author");
                    appendEntryAttributeHtml(text, item->entry, "Journal");
                    appendEntryAttributeHtml(text, item->entry, "Volume");
                    appendEntryAttributeHtml(text, item->entry, "Number");
                    appendEntryAttributeHtml(text, item->entry, "Year");
                    appendEntryAttributeHtml(text, item->entry, "Note");
                    if (!item->entry.BibFile.empty())
                    {
                        text.append("<tr><td><b>BibTeX file: </b></td><td>");
                        text.append(item->entry.BibFile);
                        text.append("</td></tr>");
                    }
                    text.append("<br>");
                    break;
                case DT_BIBTEX:
                    text.append(item->entry.FullText);
                    text.append("\r\n\r\n\r\n"); //TODO/**/
                    break;
                case DT_BIBITEM:
                    foreach (QString s, QStringList() << "author" << "title" << "journal" << "volume" << "number" << "year" << "note" )
                        bibitemtpl.replace("%"+s, QString::fromUtf8( item->entry.getAttribute(s.toUtf8().constData()).c_str()));
                    text.append(bibitemtpl.toUtf8().constData());
                    text.append("\r\n"); // TODO
                    break;
                default:
                    text.append("typenotfound"); //TODO/**/
                    break;
                }


            }
        }
    }

    if (detailViewType == DT_TEXT)
    {
        text.append("</table></html>");
        ui->txtDetail->setHtml(QString::fromUtf8(text.c_str()));
    }
    else
    {
        ui->txtDetail->setText(QString::fromUtf8(text.c_str()));
    }

}

QString MainWindow::BibitemFormat;
QString MainWindow::textEditorCmdLine;

void MainWindow::showSettings()
{
    SettingsDialog s;

    if (s.exec() == QDialog::Accepted)
    {
        //textEditorCmdLine = s.textEditor;
        //BibitemFormat = s.BibitemFormat;
        qDebug() << "accept";
    } else
        qDebug() << "reject";
}
void MainWindow::detailTypeText()
{
    detailViewType = DT_TEXT;
    refreshDetail();
}

void MainWindow::detailTypeBibtex()
{
    detailViewType = DT_BIBTEX;
    refreshDetail();
}
void MainWindow::detailTypeBibitem()
{
    detailViewType = DT_BIBITEM;
    refreshDetail();
}
void MainWindow::openExternally()
{
    QModelIndex ind = theFilter->mapToSource( ui->trvFiles->selectionModel()->currentIndex() );
    TreeItem* item = static_cast<TreeItem*>(ind.internalPointer());

    if (!item )
        return;
    if (item->entry.BibFile.empty())
        return;

    if (textEditorCmdLine.isEmpty())
    {
        QMessageBox mb(this);
        mb.setText("No external editor defined. To open a BibTeX file in an external editor, please specify an editor of your choice in the settings.");
        mb.setWindowTitle("No editor specified");
        mb.exec();
    }
    else
    {
        //QString cmd =
        QProcess* p = new QProcess();
        QString cmd = textEditorCmdLine;
        cmd.replace(QString("%f"), "\"" + QString::fromUtf8( item->entry.BibFile.c_str()) + "\"");
        cmd.replace(QString("%l"), QString::number(item->entry.lineNr));
        p->start(cmd);
       // p.waitForStarted();
        //p->start("calc.exe");
        //p.waitForStarted();
        statusBar()->showMessage("Running "+cmd+"...");
    }
}

void MainWindow::copyEntry()
{
    //qDebug() << "Hallo Welt";
    std::string str;
    QModelIndexList lst = ui->trvFiles->selectionModel()->selectedIndexes();
    ///std::cout << "+++";
    int count=0;
    foreach (QModelIndex ind, lst)
    {
    //QModelIndex ind = ui->trvFiles->selectionModel()->currentIndex()
        if (ind.column() ==0)
        {
            QModelIndex sind = theFilter->mapToSource(ind);
             //qDebug() << "**";
            TreeItem *item = static_cast<TreeItem*>(sind.internalPointer());
            if (item && item->entry.type==0)
            {
                count++;
                //qDebug() << QString::fromUtf8(item->entry.entrykey.c_str());
                str.append(item->entry.FullText);
                str.append("\r\n"); //TODO/**/
            }
        }
    }

    if (!str.empty())
    {
        QClipboard *clipboard = QApplication::clipboard();
        //QString originalText = clipboard->text();
        clipboard->setText(QString::fromUtf8(str.c_str()));

        statusBar()->showMessage(QString::number( count) + tr(" entrie(s) copied to clipboard"));
    }
    else
    {
        statusBar()->showMessage("Nothing to copy to clipboard.");
    }

}

void MainWindow::OnTrvContextMenuRequested(const QPoint & pos)
{
    QMenu menu(this);
    menu.addAction(CopyEntryAct);
    menu.addAction(OpenExternallyAct);
    menu.addSeparator();
    QMenu *dmenu = menu.addMenu(tr("Detail View"));
    dmenu->addAction(DetailTextAct);
    dmenu->addAction(DetailBibtexAct);
    dmenu->addAction(DetailBibitemAct);
    menu.addSeparator();
    menu.addAction(SettingsAct);


    menu.exec(ui->trvFiles->mapToGlobal(pos));
}

// Recursively adds BibTex files
void MainWindow::AppendAllBibFiles(QString folder, QStringList& lst)
{
    QDir dir(folder);
    QStringList lstBib = dir.entryList(QStringList("*.bib"));
    for( QStringList::ConstIterator entry=lstBib.begin(); entry!=lstBib.end(); ++entry )
    {
        //std::cout << *entry << std::endl;
        QString bibname = *entry;
        if(bibname != tr(".") && bibname != tr(".."))
        {
            lst.append(dir.absoluteFilePath(bibname));
        }
    }

    QDir dir2(folder);
    dir2.setFilter(QDir::Dirs);
    QStringList lstDirs = dir2.entryList();
    for( QStringList::ConstIterator entry=lstDirs.begin(); entry!=lstDirs.end(); ++entry )
    {
        //std::cout << *entry << std::endl;
        QString dirname = *entry;
        if(dirname != tr(".") && dirname != tr(".."))
        {
            AppendAllBibFiles(dir2.absoluteFilePath(dirname), lst);
            //qDebug() <<  dirname;
        }
    }
}

void MainWindow::FillTree()
{
    QStringList bibFiles;
    AppendAllBibFiles(ui->txtFolder->text(), bibFiles);
    //QFile file("default.txt");
    //file.open(QIODevice::ReadOnly);
    //TreeModel model(file.readAll());
    //TreeModel* tmodel = new TreeModel("hallo\n welt\n welt\nbla\n blupppppp");
    //file.close();

    //QTreeView view;
    model = new TreeModel(bibFiles);
    theFilter = new MyTreeProxyFilter(this);
    theFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    theFilter->setSourceModel(model);
    //ui->trvFiles->setModel(model);
    ui->trvFiles->setModel(theFilter);
    //ui->trvFiles->setModel(tmodel);
    //view.setWindowTitle(QObject::tr("Simple Tree Model"));
    //view.show();
    connect(ui->trvFiles->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(OnSelectionChanged(QItemSelection, QItemSelection)));
}

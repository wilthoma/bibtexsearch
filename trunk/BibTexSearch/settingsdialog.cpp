#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "mainwindow.h"
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->txtExternalEditor->setText( MainWindow::textEditorCmdLine);
    ui->txtBibItemFormat->setPlainText(MainWindow::BibitemFormat);
    ui->chkScanTexFiles->setChecked(MainWindow::scanTexFiles);

    connect(this, SIGNAL(accepted()), SLOT(OnAccept()));

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::OnAccept()
{
    MainWindow::textEditorCmdLine = ui->txtExternalEditor->text();
    MainWindow::BibitemFormat = ui->txtBibItemFormat->toPlainText();
    MainWindow::scanTexFiles = ui->chkScanTexFiles->isChecked();
}

void SettingsDialog::closeEvent(QCloseEvent *event)
{
    //textEditor = ui->txtExternalEditor->text();
    //BibitemFormat = ui->txtBibItemFormat->toPlainText();
    qDebug() << "closeevent settings";
}

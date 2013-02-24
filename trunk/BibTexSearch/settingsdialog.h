#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void closeEvent(QCloseEvent *event);
    
    QString textEditor, BibitemFormat;

private slots:
    void OnAccept();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H

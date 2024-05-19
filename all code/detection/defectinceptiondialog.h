#ifndef DEFECTINCEPTIONDIALOG_H
#define DEFECTINCEPTIONDIALOG_H

#include <QDialog>

namespace Ui {
class DefectInceptionDialog;
}

class DefectInceptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DefectInceptionDialog(QWidget *parent = nullptr);
    ~DefectInceptionDialog();
public:
    QString GetInceptionMode();
private:
    void SetUpConnection();
    void SetUpBasic();
private:
    Ui::DefectInceptionDialog *ui;
};

#endif // DEFECTINCEPTIONDIALOG_H

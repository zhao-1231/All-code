#ifndef MEASUREMENTSETTINGDIALOG_H
#define MEASUREMENTSETTINGDIALOG_H
#pragma execution_character_set("utf-8")
#include <QDialog>

namespace Ui {
class MeasureMentSettingDialog;
}

class MeasureMentSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MeasureMentSettingDialog(QWidget *parent = nullptr);
    ~MeasureMentSettingDialog();
    QVector<int> GetGroup1ValidIndex();
    void SetGroup1ValidIndex(QVector<int>& vals);
private:
    void SetUpConnections();
private:
    Ui::MeasureMentSettingDialog *ui;
};

#endif // MEASUREMENTSETTINGDIALOG_H

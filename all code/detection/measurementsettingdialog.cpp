#include "measurementsettingdialog.h"
#include "ui_measurementsettingdialog.h"

MeasureMentSettingDialog::MeasureMentSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasureMentSettingDialog)
{
    ui->setupUi(this);
    setWindowTitle("拼接参数设置");
    setWindowIcon(QIcon{":/icons/setting1.png"});
    SetUpConnections();
}

MeasureMentSettingDialog::~MeasureMentSettingDialog()
{
    delete ui;
}


QVector<int> MeasureMentSettingDialog::GetGroup1ValidIndex()
{
    int idx1 = ui->m_G1index1->text().toInt();
    int idx2 = ui->m_G1index2->text().toInt();
    int idx3 = ui->m_G1index3->text().toInt();
    QVector<int> res;
    res.push_back(idx1);
    res.push_back(idx2);
    res.push_back(idx3);
    return res;
}

void MeasureMentSettingDialog::SetGroup1ValidIndex(QVector<int>& vals)
{
    QString str;
    ui->m_G1index1->setText(str.setNum(vals[0]));
    ui->m_G1index2->setText(str.setNum(vals[1]));
    ui->m_G1index3->setText(str.setNum(vals[2]));
}

void MeasureMentSettingDialog::SetUpConnections()
{
    connect(ui->m_applyBtn,&QPushButton::clicked,this,&MeasureMentSettingDialog::accept);
}

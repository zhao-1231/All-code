#include "defectinceptiondialog.h"
#include "ui_defectinceptiondialog.h"

DefectInceptionDialog::DefectInceptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DefectInceptionDialog)
{
    ui->setupUi(this);
    SetUpBasic();
    SetUpConnection();
}

DefectInceptionDialog::~DefectInceptionDialog()
{
    delete ui;
}

void DefectInceptionDialog::SetUpConnection()
{
    connect(ui->m_ApplyBtn,&QPushButton::clicked,this,&DefectInceptionDialog::accept);
}

void DefectInceptionDialog::SetUpBasic()
{
    ui->m_ModeComboBox->addItems({"Single Inception","Squential Inception"});
}

QString DefectInceptionDialog::GetInceptionMode()
{
    return ui->m_ModeComboBox->currentText();
}





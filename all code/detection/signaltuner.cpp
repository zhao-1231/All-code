#include "signaltuner.h"
#include "ui_signaltuner.h"

SignalTuner::SignalTuner(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignalTuner)
{
    ui->setupUi(this);
    setWindowTitle("触发信号设置");
    setWindowIcon(QIcon{":/icons/radio-waves.png"});
    ui->m_G1Mode->addItems(QStringList{"先分频后倍频","先倍频后分频"});
    ui->m_G2Mode->addItems(QStringList{"先分频后倍频","先倍频后分频"});
    SetUpConnections();
    SetSlidersRange();
}

SignalTuner::~SignalTuner()
{
    delete ui;
}

void SignalTuner::SetUpConnections()
{
    connect(ui->m_G1FactorSlider,SIGNAL(valueChanged(int)),this,SLOT(G1FactorChanged(int)));
    connect(ui->m_G1DividerSlider,SIGNAL(valueChanged(int)),this,SLOT(G1DividerChanged(int)));
    connect(ui->m_G2FactorSlider,SIGNAL(valueChanged(int)),this,SLOT(G2FactorChanged(int)));
    connect(ui->m_G2DividerSlider,SIGNAL(valueChanged(int)),this,SLOT(G1DividerChanged(int)));
    connect(ui->m_ApplyBtn,SIGNAL(clicked()),this,SLOT(ApplyCurrentSettingSlot()));
    connect(ui->m_SaveBtn,SIGNAL(clicked()),this,SLOT(SaveCurrentSettingSlot()));
    connect(ui->m_LoadBtn,SIGNAL(clicked()),this,SLOT(LoadExistingSettingSlot()));
    //connect(ui->m_G1LineAcquire,signal(),this,SLOT());
}


void SignalTuner::SetSlidersRange()
{
    ui->m_G1FactorSlider->setRange(0,8);
    ui->m_G2FactorSlider->setRange(0,8);
    ui->m_G1DividerSlider->setRange(1,255);
    ui->m_G2DividerSlider->setRange(1,255);
    ui->m_G1LineAcquire->setRange(1,15);
    ui->m_G2LineAcquire->setRange(1,15);
    ui->m_G1FactorSlider->setDisabled(true);
    ui->m_G2FactorSlider->setDisabled(true);
    ui->m_G1DividerSlider->setDisabled(true);
    ui->m_G2DividerSlider->setDisabled(true);
    ui->m_G1CurVal->setDisabled(true);
    ui->m_G2CurVal->setDisabled(true);
}


void SignalTuner::G1FactorChanged(int val)
{
    m_G1FactorVal = val;
    QString str;
    ui->m_G1CurVal->setText(str.setNum((powf(2.,m_G1FactorVal)/float(m_G1DividerVal))));
}
void SignalTuner::G1DividerChanged(int val)
{
    m_G1DividerVal = val;
    QString str;
    ui->m_G1CurVal->setText(str.setNum((powf(2.,m_G1FactorVal)/float(m_G1DividerVal))));
}
void SignalTuner::G2FactorChanged(int val)
{
    m_G2FactorVal = val;
    QString str;
    ui->m_G2CurVal->setText(str.setNum((powf(2.,m_G2FactorVal)/float(m_G2DividerVal))));
}
void SignalTuner::G2DividerChanged(int val)
{
    m_G2DividerVal = val;
    QString str;
    ui->m_G2CurVal->setText(str.setNum((powf(2.,m_G2FactorVal)/float(m_G2DividerVal))));
}

void SignalTuner::ApplyCurrentSettingSlot()
{
    m_G1LinePerSignalVal = ui->m_G1LineAcquire->value();
    m_G2LinePerSignalVal = ui->m_G2LineAcquire->value();
    for(int i{};i<Cameras.size();++i){
        CameraBasic* elem = Cameras.at(i);
        elem->SetFeatureVal("rotaryEncoderMultiplier",int(powf(2.,m_G1FactorVal)));
        elem->SetFeatureVal("rotaryEncoderDivider",m_G1DividerVal);
        if(elem->DisConnectionTransfer()){
            elem->SetFeatureVal("triggerLineCount",ui->m_G1LineAcquire->value());
            elem->ConnectTransfer();
            QString str;
            str.sprintf("[相机] %s 设置应用成功",elem->GetCameraVender().toStdString().c_str());
            sendToLogger(str,Qt::green);
        }
    }
    for(int i{CamNumsPerGroup};i<Cameras.size();++i){
        CameraBasic* elem = Cameras.at(i);
        elem->SetFeatureVal("rotaryEncoderMultiplier",int(powf(2.,m_G2FactorVal)));
        elem->SetFeatureVal("rotaryEncoderDivider",m_G2DividerVal);
        elem->SetFeatureVal("triggerLineCount",ui->m_G2LineAcquire->value());
        if(elem->DisConnectionTransfer()){
            elem->SetFeatureVal("triggerLineCount",ui->m_G1LineAcquire->value());
            elem->ConnectTransfer();
            QString str;
            str.sprintf("[相机] %s 设置应用成功",elem->GetCameraVender().toStdString().c_str());
            sendToLogger(str,Qt::green);
        }
    }
}

void SignalTuner::SaveCurrentSettingSlot()
{

}

void SignalTuner::LoadExistingSettingSlot()
{

}



void SignalTuner::updateVal()
{
    CameraBasic* G1Cam = nullptr;
    if(Cameras.size() != 0){
        G1Cam = Cameras[0];
        ui->m_G1FactorSlider->setEnabled(true);
        ui->m_G1DividerSlider->setEnabled(true);
    }
    CameraBasic* G2Cam = nullptr;
    if(Cameras.size() > CamNumsPerGroup){
        G2Cam = Cameras[CamNumsPerGroup];
        ui->m_G2FactorSlider->setEnabled(true);
        ui->m_G2DividerSlider->setEnabled(true);
    }
    if(G1Cam != nullptr){
        G1Cam->GetFeatureVal("rotaryEncoderMultiplier",m_G1FactorVal);
        G1Cam->GetFeatureVal("rotaryEncoderDivider",m_G1DividerVal);
        G1Cam->GetFeatureVal("triggerLineCount",m_G1LinePerSignalVal);
        QString str;
        ui->m_G1CurVal->setText(str.setNum((float(m_G1FactorVal)/float(m_G1DividerVal))));
        ui->m_G1LineAcquire->setValue(m_G1LinePerSignalVal);
        ui->m_G1FactorSlider->setValue(int(std::log(m_G1FactorVal) / std::log(2)));
        ui->m_G1DividerSlider->setValue(m_G1DividerVal);
    }
    if(G2Cam != nullptr){
        G2Cam->GetFeatureVal("rotaryEncoderMultiplier",m_G2FactorVal);
        G2Cam->GetFeatureVal("rotaryEncoderDivider",m_G2DividerVal);
        G2Cam->GetFeatureVal("triggerLineCount",m_G2LinePerSignalVal);
        QString str;
        ui->m_G2CurVal->setText(str.setNum(powf(2.,m_G2FactorVal)/m_G2DividerVal));
        ui->m_G2LineAcquire->setValue(m_G2LinePerSignalVal);
        ui->m_G2FactorSlider->setValue(int(std::log(m_G2FactorVal) / std::log(2)));
        ui->m_G2DividerSlider->setValue(m_G2DividerVal);
    }

}
























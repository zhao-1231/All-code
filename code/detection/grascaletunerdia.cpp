#include "grascaletunerdia.h"
#include "ui_grascaletunerdia.h"

GrascaleTunerDia::GrascaleTunerDia(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrascaleTunerDia)
{

    ui->setupUi(this);
    setWindowTitle("灰度设置");
    setWindowIcon(QIcon{":/icons/Tuner.png"});
    setUpConnections();
    SetSlideValRange();
    //qDebug()<<"..........................................";
}

GrascaleTunerDia::~GrascaleTunerDia()
{
    delete ui;
}

void GrascaleTunerDia::setUpConnections()
{
    connect(ui->m_G1CamAfftBtn,SIGNAL(clicked()),this,SLOT(PickG1CamAFFTSlot()));
    connect(ui->m_G1CamBfftBtn,SIGNAL(clicked()),this,SLOT(PickG1CamBFFTSlot()));
    connect(ui->m_G1CamCfftBtn,SIGNAL(clicked()),this,SLOT(PickG1CamCFFTSlot()));
    connect(ui->m_G1CamDfftBtn,SIGNAL(clicked()),this,SLOT(PickG1CamDFFTSlot()));
    //connect(ui->m_G1ApplyBtn,SIGNAL(clicked()),this,SLOT(ApplyG1SettingsSlot()));
    connect(ui->m_G1ApplyBtn,&QPushButton::clicked,this,&GrascaleTunerDia::accept);


    connect(ui->m_G2CamAfftBtn,SIGNAL(clicked()),this,SLOT(PickG2CamAFFTSlot()));
    connect(ui->m_G2CamBfftBtn,SIGNAL(clicked()),this,SLOT(PickG2CamBFFTSlot()));
    connect(ui->m_G2CamCfftBtn,SIGNAL(clicked()),this,SLOT(PickG2CamCFFTSlot()));
    connect(ui->m_G2CamDfftBtn,SIGNAL(clicked()),this,SLOT(PickG2CamDFFTSlot()));
    connect(ui->m_G2ApplyBtn,&QPushButton::clicked,this,&GrascaleTunerDia::accept);


    connect(ui->m_G1CamASlider,SIGNAL(valueChanged(int)),this,SLOT(G1CamAGainChangedSkot(int)));
    connect(ui->m_G1CamBSlider,SIGNAL(valueChanged(int)),this,SLOT(G1CamBGainChangedSkot(int)));
    connect(ui->m_G1CamCSlider,SIGNAL(valueChanged(int)),this,SLOT(G1CamCGainChangedSkot(int)));
    connect(ui->m_G1CamDSlider,SIGNAL(valueChanged(int)),this,SLOT(G1CamDGainChangedSkot(int)));

    connect(ui->m_G2CamASlider,SIGNAL(valueChanged(int)),this,SLOT(G2CamAGainChangedSkot(int)));
    connect(ui->m_G2CamBSlider,SIGNAL(valueChanged(int)),this,SLOT(G2CamBGainChangedSkot(int)));
    connect(ui->m_G2CamCSlider,SIGNAL(valueChanged(int)),this,SLOT(G2CamCGainChangedSkot(int)));
    connect(ui->m_G2CamDSlider,SIGNAL(valueChanged(int)),this,SLOT(G2CamDGainChangedSkot(int)));

    connect(ui->m_SaveSettingsBtn,SIGNAL(clicked()),this,SLOT(SaveSettingSlot()));
    connect(ui->m_LoadSettingsBtn,SIGNAL(clicked()),this,SLOT(LoadSettingSlot()));

}

void GrascaleTunerDia::SetSlideValRange()
{
    ui->m_G1CamASlider->setRange(0,620);
    ui->m_G1CamBSlider->setRange(0,620);
    ui->m_G1CamCSlider->setRange(0,620);
    ui->m_G1CamDSlider->setRange(0,620);
    ui->m_G2CamASlider->setRange(0,620);
    ui->m_G2CamBSlider->setRange(0,620);
    ui->m_G2CamCSlider->setRange(0,620);
    ui->m_G2CamDSlider->setRange(0,620);

}

void GrascaleTunerDia::SetInitialPathState(QStringList& theElem,int mode)
{
    switch (mode) {
    case 1:
    {
        ui->m_G1CamAfft->setText(theElem[0]);
        ui->m_G1CamBfft->setText(theElem[1]);
        ui->m_G1CamCfft->setText(theElem[2]);
        ui->m_G1CamDfft->setText(theElem[3]);
    }
    case 2:
    {
        ui->m_G2CamAfft->setText(theElem[4]);
        ui->m_G2CamBfft->setText(theElem[5]);
        ui->m_G2CamCfft->setText(theElem[6]);
        ui->m_G2CamDfft->setText(theElem[7]);
    }
    default:
        break;
    }

}

void GrascaleTunerDia::SetInitialGainState(QVector<double>& theElem,int mode)
{
    switch (mode) {
    case 1:
    {
        QString str;
        ui->m_G1CamASlider->setValue(int((theElem[0]-0.125)/ 0.016));
        ui->m_G1CamAGainVal->setText(str.setNum(theElem[0]));
        ui->m_G1CamBSlider->setValue(int((theElem[1]-0.125)/ 0.016));
        ui->m_G1CamBGainVal->setText(str.setNum(theElem[1]));
        ui->m_G1CamCSlider->setValue(int((theElem[2]-0.125)/ 0.016));
        ui->m_G1CamCGainVal->setText(str.setNum(theElem[2]));
        ui->m_G1CamDSlider->setValue(int((theElem[3]-0.125)/ 0.016));
        ui->m_G1CamDGainVal->setText(str.setNum(theElem[3]));
    }
    case 2:
    {
        QString str;
        ui->m_G2CamASlider->setValue(int((theElem[4]-0.125)/ 0.016));
        ui->m_G2CamAGainVal->setText(str.setNum(theElem[4]));
        ui->m_G2CamBSlider->setValue(int((theElem[5]-0.125)/ 0.016));
        ui->m_G2CamBGainVal->setText(str.setNum(theElem[5]));
        ui->m_G2CamCSlider->setValue(int((theElem[6]-0.125)/ 0.016));
        ui->m_G2CamCGainVal->setText(str.setNum(theElem[6]));
        ui->m_G2CamDSlider->setValue(int((theElem[7]-0.125)/ 0.016));
        ui->m_G2CamDGainVal->setText(str.setNum(theElem[7]));
    }
    default:
        break;
    }
}

QString GrascaleTunerDia::GetPickedFlatFiledPath()
{
    QString file_path = QFileDialog::getOpenFileName(this,"选择平场文件",QString(),"flat fieldFile (*.tif)");
    return file_path;
}

void GrascaleTunerDia::PickG1CamAFFTSlot()
{
    QString file_path = GetPickedFlatFiledPath();
    ui->m_G1CamAfft->setText(file_path);
}


void GrascaleTunerDia::PickG1CamBFFTSlot()
{
    QString file_path = GetPickedFlatFiledPath();
    ui->m_G1CamBfft->setText(file_path);
}

void GrascaleTunerDia::PickG1CamCFFTSlot()
{
    QString file_path = GetPickedFlatFiledPath();
    ui->m_G1CamCfft->setText(file_path);
}
void GrascaleTunerDia::PickG1CamDFFTSlot()
{
    QString file_path = GetPickedFlatFiledPath();
    ui->m_G1CamDfft->setText(file_path);
}
void GrascaleTunerDia::PickG2CamAFFTSlot()
{
    QString file_path = GetPickedFlatFiledPath();
    ui->m_G2CamAfft->setText(file_path);
}
void GrascaleTunerDia::PickG2CamBFFTSlot()
{
    QString file_path = GetPickedFlatFiledPath();
    ui->m_G2CamBfft->setText(file_path);
}
void GrascaleTunerDia::PickG2CamCFFTSlot()
{
    QString file_path = GetPickedFlatFiledPath();
    ui->m_G2CamCfft->setText(file_path);
}
void GrascaleTunerDia::PickG2CamDFFTSlot()
{
    QString file_path = GetPickedFlatFiledPath();
    ui->m_G2CamDfft->setText(file_path);
}


void GrascaleTunerDia::ApplyG1SettingsSlot()
{
    Group1FFtPaths.clear();
    Group1FFtPaths.push_back(ui->m_G1CamAfft->text());
    Group1FFtPaths.push_back(ui->m_G1CamBfft->text());
    Group1FFtPaths.push_back(ui->m_G1CamCfft->text());
    Group1FFtPaths.push_back(ui->m_G1CamDfft->text());
    Group1Gains.clear();
    Group1Gains.push_back(ui->m_G1CamAGainVal->text().toFloat());
    Group1Gains.push_back(ui->m_G1CamBGainVal->text().toFloat());
    Group1Gains.push_back(ui->m_G1CamCGainVal->text().toFloat());
    Group1Gains.push_back(ui->m_G1CamDGainVal->text().toFloat());
    G1Ok = true;
    ui->m_logger->setTextColor(Qt::green);
    ui->m_logger->append("Group One Apply Seetings...");
}

void GrascaleTunerDia::ApplyG2SettingsSlot()
{
    Group2FFtPaths.clear();
    Group2FFtPaths.push_back(ui->m_G2CamAfft->text());
    Group2FFtPaths.push_back(ui->m_G2CamBfft->text());
    Group2FFtPaths.push_back(ui->m_G2CamCfft->text());
    Group2FFtPaths.push_back(ui->m_G2CamDfft->text());
    Group2Gains.clear();
    Group2Gains.push_back(ui->m_G2CamAGainVal->text().toFloat());
    Group2Gains.push_back(ui->m_G2CamBGainVal->text().toFloat());
    Group2Gains.push_back(ui->m_G2CamCGainVal->text().toFloat());
    Group2Gains.push_back(ui->m_G2CamDGainVal->text().toFloat());
    G2OK = true;
    ui->m_logger->setTextColor(Qt::green);
    ui->m_logger->append("Group Two Apply Seetings...");
}

void GrascaleTunerDia::G1CamAGainChangedSkot(int val)
{
    QString str;
    float gain = val * 0.016 + 0.125;
    ui->m_G1CamAGainVal->setText(str.setNum(gain));
}
void GrascaleTunerDia::G1CamBGainChangedSkot(int val)
{
    QString str;
    float gain = val * 0.016 + 0.125;
    ui->m_G1CamBGainVal->setText(str.setNum(gain));
}
void GrascaleTunerDia::G1CamCGainChangedSkot(int val)
{
    QString str;
    float gain = val * 0.016 + 0.125;
    ui->m_G1CamCGainVal->setText(str.setNum(gain));
}
void GrascaleTunerDia::G1CamDGainChangedSkot(int val)
{
    QString str;
    float gain = val * 0.016 + 0.125;
    ui->m_G1CamDGainVal->setText(str.setNum(gain));
}
void GrascaleTunerDia::G2CamAGainChangedSkot(int val)
{
    QString str;
    float gain = val * 0.016 + 0.125;
    ui->m_G2CamAGainVal->setText(str.setNum(gain));
}
void GrascaleTunerDia::G2CamBGainChangedSkot(int val)
{
    QString str;
    float gain = val * 0.016 + 0.125;
    ui->m_G2CamBGainVal->setText(str.setNum(gain));
}
void GrascaleTunerDia::G2CamCGainChangedSkot(int val)
{
    QString str;
    float gain = val * 0.016 + 0.125;
    ui->m_G2CamCGainVal->setText(str.setNum(gain));
}
void GrascaleTunerDia::G2CamDGainChangedSkot(int val)
{
    QString str;
    float gain = val * 0.016 + 0.125;
    ui->m_G2CamDGainVal->setText(str.setNum(gain));
}


QStringList GrascaleTunerDia::GetFFtPathsSettings(int mode)
{
    switch (mode) {
    case 1:
    {
        return Group1FFtPaths;
        break;
    }
    case 2:
    {
        return Group2FFtPaths;
        break;
    }
    default:
        return QStringList{};
        break;
    }
    return QStringList{};
}

QVector<double> GrascaleTunerDia::GetGainValsSettings(int mode)
{
    switch (mode) {
    case 1:
    {
        return Group1Gains;
    }
    case 2:
    {
        return Group2Gains;
    }
    default:
        return QVector<double>{};
        break;
    }
    return QVector<double>{};
}


void GrascaleTunerDia::SaveSettingSlot()
{
    QString SaveDir = QFileDialog::getExistingDirectory(this,"保存灰度设置",QString());
    QString savedFileName = SaveDir + "/" + "CameraGrayTunerConfig.json";
    //qDebug()<<"file name: "<<savedFileName;
    QFile file{savedFileName};
    if(file.open(QIODevice::ReadWrite | QFile::Truncate)){
        //qDebug()<<"open OK";
        QJsonArray cur_array;
        QJsonObject G1_tuneJson;
        G1_tuneJson.insert("CameraA",ui->m_G1CamAGainVal->text().toDouble());
        G1_tuneJson.insert("CameraB",ui->m_G1CamBGainVal->text().toDouble());
        G1_tuneJson.insert("CameraC",ui->m_G1CamCGainVal->text().toDouble());
        G1_tuneJson.insert("CameraD",ui->m_G1CamDGainVal->text().toDouble());
        cur_array.append(G1_tuneJson);
        QJsonObject G2_tuneJson;
        G2_tuneJson.insert("CameraA",ui->m_G2CamAGainVal->text().toDouble());
        G2_tuneJson.insert("CameraB",ui->m_G2CamBGainVal->text().toDouble());
        G2_tuneJson.insert("CameraC",ui->m_G2CamCGainVal->text().toDouble());
        G2_tuneJson.insert("CameraD",ui->m_G2CamDGainVal->text().toDouble());
        cur_array.append(G2_tuneJson);
        QJsonDocument cur_doc;
        cur_doc.setArray(cur_array);
        file.write(cur_doc.toJson());
        file.close();
        ui->m_logger->append("Save Settings OK...");
    }
}

void GrascaleTunerDia::LoadSettingSlot()
{
    QString setting_names = QFileDialog::getOpenFileName(this,"加载灰度设置文件",QString(),"Json File(*.json)");
    QFile file{setting_names};
    if(!file.open(QIODevice::ReadOnly)){
        ui->m_logger->append("open Settings file FAIL...");
        return;
    }
    QJsonParseError pErr;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),&pErr);
    if(doc.isNull()){
        ui->m_logger->append("prase json File FAILED...");
        file.close();
        return;
    }

    QJsonArray array = doc.array();
    qDebug()<<"array cout: "<<array.count();
    Q_ASSERT(array.count() == 2);
    auto G1_tuneSetting = array.at(0);
    QString str;
    ui->m_G1CamAGainVal->setText(str.setNum(G1_tuneSetting["CameraA"].toDouble()));
    ui->m_G1CamBGainVal->setText(str.setNum(G1_tuneSetting["CameraB"].toDouble()));
    ui->m_G1CamCGainVal->setText(str.setNum(G1_tuneSetting["CameraC"].toDouble()));
    ui->m_G1CamDGainVal->setText(str.setNum(G1_tuneSetting["CameraD"].toDouble()));

    auto G2_tuneSetting = array.at(1);
    ui->m_G2CamAGainVal->setText(str.setNum(G2_tuneSetting["CameraA"].toDouble()));
    ui->m_G2CamBGainVal->setText(str.setNum(G2_tuneSetting["CameraB"].toDouble()));
    ui->m_G2CamCGainVal->setText(str.setNum(G2_tuneSetting["CameraC"].toDouble()));
    ui->m_G2CamDGainVal->setText(str.setNum(G2_tuneSetting["CameraD"].toDouble()));
    ui->m_logger->append("Load Exiting Settings SUCCESS...");
}














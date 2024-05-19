#include "cameradetectdialog.h"
#include "ui_cameradetectdialog.h"

CameraDetectDialog::CameraDetectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraDetectDialog)
{
    ui->setupUi(this);
    setWindowTitle("相机检测和设置");
    setWindowIcon(QIcon{":/icons/settings.png"});
    SetUpConnections();
    DisableCameraSettingOption();
    m_CamCfgDialog = new CameraConfigDlg{CameraServerNames,this};
    CameraVenders<<"CameraA"<<"CameraB"<<"CameraC"<<"CameraD"<<"CameraE"<<"CameraF"<<"CameraG"<<"CameraH";
}

CameraDetectDialog::~CameraDetectDialog()
{
    if(m_CamerasConfig != nullptr)
        delete[]m_CamerasConfig;
    m_CamerasConfig = nullptr;
    delete ui;
}

void CameraDetectDialog::DisableCameraSettingOption()
{
    ui->m_Cam1->setDisabled(true);
    ui->m_Cam1->setStyleSheet("background-color:red");
    ui->m_Cam2->setDisabled(true);
    ui->m_Cam2->setStyleSheet("background-color:red");
    ui->m_Cam3->setDisabled(true);
    ui->m_Cam3->setStyleSheet("background-color:red");
    ui->m_Cam4->setDisabled(true);
    ui->m_Cam4->setStyleSheet("background-color:red");
    ui->m_Cam5->setDisabled(true);
    ui->m_Cam5->setStyleSheet("background-color:red");
    ui->m_Cam6->setDisabled(true);
    ui->m_Cam6->setStyleSheet("background-color:red");
    ui->m_Cam7->setDisabled(true);
    ui->m_Cam7->setStyleSheet("background-color:red");
    ui->m_Cam8->setDisabled(true);
    ui->m_Cam8->setStyleSheet("background-color:red");

}

void CameraDetectDialog::EnableCameraSettingOption(int nums)
{
    if(nums < 1)
        return ;
    ui->m_Cam1->setEnabled(true);
    ui->m_Cam1->setStyleSheet("background-color:green");
    if(nums < 2)
        return ;
    ui->m_Cam2->setEnabled(true);
    ui->m_Cam2->setStyleSheet("background-color:green");
    //qDebug()<<"Camera nums 2 ";
    if(nums < 3)
        return ;
    ui->m_Cam3->setEnabled(true);
    ui->m_Cam3->setStyleSheet("background-color:green");
    if(nums < 4)
        return ;
    ui->m_Cam4->setEnabled(true);
    ui->m_Cam4->setStyleSheet("background-color:green");
    if(nums < 5)
        return ;
    ui->m_Cam5->setEnabled(true);
    ui->m_Cam5->setStyleSheet("background-color:green");
    if(nums < 6)
        return ;
    ui->m_Cam6->setEnabled(true);
    ui->m_Cam6->setStyleSheet("background-color:green");
    if(nums < 7)
        return ;
    ui->m_Cam7->setEnabled(true);
    ui->m_Cam7->setStyleSheet("background-color:green");
    if(nums < 8)
        return ;
    ui->m_Cam8->setEnabled(true);
    ui->m_Cam8->setStyleSheet("background-color:green");

}


void CameraDetectDialog::SetCam1ConfigsSlot()
{
    m_CamCfgDialog->updateComboBox();
    m_CamCfgDialog->setWindowTitle("相机设置(相机A)");
    if(m_CamCfgDialog->exec()){
        m_CamerasConfig[0].CameraServerName = CameraServerNames.at(m_CamCfgDialog->m_comboBox->currentIndex());
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(m_CamerasConfig[0].CameraServerName.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        m_CamerasConfig[0].CameraOwnName = deviceName;
        CameraServerNames.removeAt(m_CamCfgDialog->m_comboBox->currentIndex());
        if(!m_CamCfgDialog->m_configPathEdit->text().isEmpty())
            m_CamerasConfig[0].CameraConfigFile = m_CamCfgDialog->m_configPathEdit->text();
        if(!m_CamCfgDialog->m_FlatFieldEdit->text().isEmpty())
            m_CamerasConfig[0].CameraFlatfieldFile = m_CamCfgDialog->m_FlatFieldEdit->text();
        QString str = "CameraA: " + m_CamerasConfig[0].CameraServerName + "   config file : " + \
                m_CamerasConfig[0].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[0].CameraFlatfieldFile;
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("\n");
        ui->m_DetectedInfo->append(str);
    }
}

void CameraDetectDialog::SetCam2ConfigsSlot()
{
    m_CamCfgDialog->updateComboBox();
    m_CamCfgDialog->setWindowTitle("相机设置(相机B)");
    if(m_CamCfgDialog->exec()){
        m_CamerasConfig[1].CameraServerName = CameraServerNames.at(m_CamCfgDialog->m_comboBox->currentIndex());
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(m_CamerasConfig[1].CameraServerName.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        m_CamerasConfig[1].CameraOwnName = deviceName;
        CameraServerNames.removeAt(m_CamCfgDialog->m_comboBox->currentIndex());
        if(!m_CamCfgDialog->m_configPathEdit->text().isEmpty())
            m_CamerasConfig[1].CameraConfigFile = m_CamCfgDialog->m_configPathEdit->text();
        if(!m_CamCfgDialog->m_FlatFieldEdit->text().isEmpty())
            m_CamerasConfig[1].CameraFlatfieldFile = m_CamCfgDialog->m_FlatFieldEdit->text();
        QString str = "CameraB: " + m_CamerasConfig[1].CameraServerName + "   config file : " + \
                m_CamerasConfig[1].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[1].CameraFlatfieldFile;
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("\n");
        ui->m_DetectedInfo->append(str);
    }
}

void CameraDetectDialog::SetCam3ConfigsSlot()
{
    m_CamCfgDialog->updateComboBox();
    m_CamCfgDialog->setWindowTitle("相机设置(相机C)");
    if(m_CamCfgDialog->exec()){
        m_CamerasConfig[2].CameraServerName = CameraServerNames.at(m_CamCfgDialog->m_comboBox->currentIndex());
        CameraServerNames.removeAt(m_CamCfgDialog->m_comboBox->currentIndex());
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(m_CamerasConfig[2].CameraServerName.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        m_CamerasConfig[2].CameraOwnName = deviceName;
        if(!m_CamCfgDialog->m_configPathEdit->text().isEmpty())
            m_CamerasConfig[2].CameraConfigFile = m_CamCfgDialog->m_configPathEdit->text();
        if(!m_CamCfgDialog->m_FlatFieldEdit->text().isEmpty())
            m_CamerasConfig[2].CameraFlatfieldFile = m_CamCfgDialog->m_FlatFieldEdit->text();
        QString str = "CameraC: " + m_CamerasConfig[2].CameraServerName + "   config file : " + \
                m_CamerasConfig[2].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[2].CameraFlatfieldFile;
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("\n");
        ui->m_DetectedInfo->append(str);
    }

}

void CameraDetectDialog::SetCam4ConfigsSlot()
{
    m_CamCfgDialog->updateComboBox();
    m_CamCfgDialog->setWindowTitle("相机设置(相机D)");
    if(m_CamCfgDialog->exec()){
        m_CamerasConfig[3].CameraServerName = CameraServerNames.at(m_CamCfgDialog->m_comboBox->currentIndex());
        CameraServerNames.removeAt(m_CamCfgDialog->m_comboBox->currentIndex());
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(m_CamerasConfig[3].CameraServerName.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        m_CamerasConfig[3].CameraOwnName = deviceName;
        if(!m_CamCfgDialog->m_configPathEdit->text().isEmpty())
            m_CamerasConfig[3].CameraConfigFile = m_CamCfgDialog->m_configPathEdit->text();
        if(!m_CamCfgDialog->m_FlatFieldEdit->text().isEmpty())
            m_CamerasConfig[3].CameraFlatfieldFile = m_CamCfgDialog->m_FlatFieldEdit->text();
        QString str = "CameraD: " + m_CamerasConfig[3].CameraServerName + "   config file : " + \
                m_CamerasConfig[3].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[3].CameraFlatfieldFile;
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("\n");
        ui->m_DetectedInfo->append(str);
    }
}

void CameraDetectDialog::SetCam5ConfigsSlot()
{
    m_CamCfgDialog->updateComboBox();
    m_CamCfgDialog->setWindowTitle("相机设置(相机E)");
    if(m_CamCfgDialog->exec()){
        m_CamerasConfig[4].CameraServerName = CameraServerNames.at(m_CamCfgDialog->m_comboBox->currentIndex());
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(m_CamerasConfig[4].CameraServerName.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        m_CamerasConfig[4].CameraOwnName = deviceName;
        CameraServerNames.removeAt(m_CamCfgDialog->m_comboBox->currentIndex());
        if(!m_CamCfgDialog->m_configPathEdit->text().isEmpty())
            m_CamerasConfig[4].CameraConfigFile = m_CamCfgDialog->m_configPathEdit->text();
        if(!m_CamCfgDialog->m_FlatFieldEdit->text().isEmpty())
            m_CamerasConfig[4].CameraFlatfieldFile = m_CamCfgDialog->m_FlatFieldEdit->text();
        QString str = "CameraE: " + m_CamerasConfig[4].CameraServerName + "   config file : " + \
                m_CamerasConfig[4].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[4].CameraFlatfieldFile;
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("\n");
        ui->m_DetectedInfo->append(str);
    }

}

void CameraDetectDialog::SetCam6ConfigsSlot()
{
    m_CamCfgDialog->updateComboBox();
    m_CamCfgDialog->setWindowTitle("相机设置(相机F)");
    if(m_CamCfgDialog->exec()){
        m_CamerasConfig[5].CameraServerName = CameraServerNames.at(m_CamCfgDialog->m_comboBox->currentIndex());
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(m_CamerasConfig[5].CameraServerName.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        m_CamerasConfig[5].CameraOwnName = deviceName;
        CameraServerNames.removeAt(m_CamCfgDialog->m_comboBox->currentIndex());
        if(!m_CamCfgDialog->m_configPathEdit->text().isEmpty())
            m_CamerasConfig[5].CameraConfigFile = m_CamCfgDialog->m_configPathEdit->text();
        if(!m_CamCfgDialog->m_FlatFieldEdit->text().isEmpty())
            m_CamerasConfig[5].CameraFlatfieldFile = m_CamCfgDialog->m_FlatFieldEdit->text();
        QString str = "CameraF: " + m_CamerasConfig[5].CameraServerName + "   config file : " + \
                m_CamerasConfig[5].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[5].CameraFlatfieldFile;
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("\n");
        ui->m_DetectedInfo->append(str);
    }

}

void CameraDetectDialog::SetCam7ConfigsSlot()
{
    m_CamCfgDialog->updateComboBox();
    m_CamCfgDialog->setWindowTitle("相机设置(相机G)");
    if(m_CamCfgDialog->exec()){
        m_CamerasConfig[6].CameraServerName = CameraServerNames.at(m_CamCfgDialog->m_comboBox->currentIndex());
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(m_CamerasConfig[6].CameraServerName.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        m_CamerasConfig[6].CameraOwnName = deviceName;
        CameraServerNames.removeAt(m_CamCfgDialog->m_comboBox->currentIndex());
        if(!m_CamCfgDialog->m_configPathEdit->text().isEmpty())
            m_CamerasConfig[6].CameraConfigFile = m_CamCfgDialog->m_configPathEdit->text();
        if(!m_CamCfgDialog->m_FlatFieldEdit->text().isEmpty())
            m_CamerasConfig[6].CameraFlatfieldFile = m_CamCfgDialog->m_FlatFieldEdit->text();
        QString str = "CameraG: " + m_CamerasConfig[6].CameraServerName + "   config file : " + \
                m_CamerasConfig[6].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[6].CameraFlatfieldFile;
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("\n");
        ui->m_DetectedInfo->append(str);
    }

}

void CameraDetectDialog::SetCam8ConfigsSlot()
{
    m_CamCfgDialog->updateComboBox();
    m_CamCfgDialog->setWindowTitle("相机设置(相机H)");
    if(m_CamCfgDialog->exec()){
        m_CamerasConfig[7].CameraServerName = CameraServerNames.at(m_CamCfgDialog->m_comboBox->currentIndex());
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(m_CamerasConfig[7].CameraServerName.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        m_CamerasConfig[7].CameraOwnName = deviceName;
        CameraServerNames.removeAt(m_CamCfgDialog->m_comboBox->currentIndex());
        if(!m_CamCfgDialog->m_configPathEdit->text().isEmpty())
            m_CamerasConfig[7].CameraConfigFile = m_CamCfgDialog->m_configPathEdit->text();
        if(!m_CamCfgDialog->m_FlatFieldEdit->text().isEmpty())
            m_CamerasConfig[7].CameraFlatfieldFile = m_CamCfgDialog->m_FlatFieldEdit->text();
        QString str = "CameraH: " + m_CamerasConfig[7].CameraServerName + "   config file : " + \
                m_CamerasConfig[7].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[7].CameraFlatfieldFile;
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("\n");
        ui->m_DetectedInfo->append(str);
    }

}

void CameraDetectDialog::EmitCamerasConfigs()
{
    emit sendCameraConfigs(camCount,m_CamerasConfig);
}

void CameraDetectDialog::SetUpConnections()
{
    connect(ui->m_DetectCamBtn,SIGNAL(clicked()),this,SLOT(DetectCamerasSlot()));
    connect(ui->m_Cam1,SIGNAL(clicked()),this,SLOT(SetCam1ConfigsSlot()));
    connect(ui->m_Cam2,SIGNAL(clicked()),this,SLOT(SetCam2ConfigsSlot()));
    connect(ui->m_Cam3,SIGNAL(clicked()),this,SLOT(SetCam3ConfigsSlot()));
    connect(ui->m_Cam4,SIGNAL(clicked()),this,SLOT(SetCam4ConfigsSlot()));
    connect(ui->m_Cam5,SIGNAL(clicked()),this,SLOT(SetCam5ConfigsSlot()));
    connect(ui->m_Cam6,SIGNAL(clicked()),this,SLOT(SetCam6ConfigsSlot()));
    connect(ui->m_Cam7,SIGNAL(clicked()),this,SLOT(SetCam7ConfigsSlot()));
    connect(ui->m_Cam8,SIGNAL(clicked()),this,SLOT(SetCam8ConfigsSlot()));
    connect(ui->m_applyBtn,SIGNAL(clicked()),this,SLOT(EmitCamerasConfigs()));
    //m_SaveSettingBtn
    connect(ui->m_SaveSettingBtn,SIGNAL(clicked()),this,SLOT(SaveCurrentConfigSlot()));
    //m_LoadSettingBtn
    connect(ui->m_LoadSettingBtn,SIGNAL(clicked()),this,SLOT(LoadCameraConfigsSlot()));

}

void CameraDetectDialog::ClearnLoggerInfo()
{
    ui->m_DetectedInfo->clear();
}

void CameraDetectDialog::DetectCamerasSlot()
{
    camCount = SapManager::GetServerCount(SapManager::ResourceAcqDevice);
    if(m_CamerasConfig != nullptr){
        delete[]m_CamerasConfig;
        m_CamerasConfig = new CameraConfigs[camCount]{};
    }else{
        m_CamerasConfig = new CameraConfigs[camCount]{};
    }
    EnableCameraSettingOption(camCount);
    ui->m_DetectedInfo->clear();
    if(camCount > 0){
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(0, 255, 0);font: 15pt "Arial";)");
        QString str;
        QString showInfo;
        showInfo = "Detect " + str.setNum(camCount) + " Cameras.";
         ui->m_DetectedInfo->append(showInfo);
        for(int i{};i<= camCount;++i){
            if(SapManager::GetResourceCount(i,SapManager::ResourceAcqDevice) != 0){
                char serverName[CORSERVER_MAX_STRLEN]{};
                SapManager::GetServerName(i,serverName);
                char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
                SapManager::GetResourceName(serverName, SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
                ui->m_DetectedInfo->append("CameraServer: " + QString{serverName} + "     CameraName: " + QString{deviceName});
                CameraServerNames.push_back(serverName);
            }
        }
    }
    else{
        ui->m_DetectedInfo->setStyleSheet(R"(color: rgb(255, 0, 0);font: 15pt "Arial";)");
        ui->m_DetectedInfo->append("No Camera Detected...");
    }

}

void CameraDetectDialog::SetInitialState()
{
    DisableCameraSettingOption();
}


void CameraDetectDialog::SaveCurrentConfigSlot()
{
    //保存相机设置json文件格式
    QString SaveDir = QFileDialog::getExistingDirectory(this,"保存相机设置",QString());
    QString savedFileName = SaveDir + "/" + "CameraConfig.json";
    qDebug()<<"file name: "<<savedFileName;
    QFile file{savedFileName};
    if(file.open(QIODevice::ReadWrite)){
        qDebug()<<"open OK";
        if(camCount > 0){
        QJsonArray cur_array;
        QJsonObject cam_numJs;
        cam_numJs.insert("CamCount",camCount);
        cur_array.append(cam_numJs);
        for(int i{};i<camCount;++i){
            QJsonObject cur_obj;
            cur_obj.insert("Camera_Vender",CameraVenders[i]);
            cur_obj.insert("Camera_ServerName",m_CamerasConfig[i].CameraServerName);
            cur_obj.insert("Camera_Name",m_CamerasConfig[i].CameraOwnName);
            cur_obj.insert("Camera_ConfigFile",m_CamerasConfig[i].CameraConfigFile);
            cur_obj.insert("Camera_FlatFile",m_CamerasConfig[i].CameraFlatfieldFile);
            cur_array.append(cur_obj);
        }
            QJsonDocument cur_doc;
            cur_doc.setArray(cur_array);
            file.write(cur_doc.toJson());
            file.close();
            emit sendToLogger("[系统] 相机设置保存成功",Qt::green);
        }else
            emit sendToLogger("[系统] 无有效文件",Qt::red);
    }else{
        emit sendToLogger("[系统] 打开文件失败",Qt::red);
    }
}


void CameraDetectDialog::LoadCameraConfigsSlot()
{
    QString setting_names = QFileDialog::getOpenFileName(this,"加载相机文件",QString(),"Json File(*.json)");
    QFile file{setting_names};
    if(!file.open(QIODevice::ReadOnly)){
        emit sendToLogger("[系统] 打开文件失败",Qt::red);
        return;
    }
    QJsonParseError pErr;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),&pErr);
    if(doc.isNull()){
        emit sendToLogger(pErr.errorString(),Qt::red);
        file.close();
        return;
    }

    QJsonArray array = doc.array();
    qDebug()<<"array cout: "<<array.count();
    int record_camNums = array.at(0)["CamCount"].toInt();
    if(record_camNums != camCount){
        emit sendToLogger("[系统] 加载文件相机数量与当前数量不符合",Qt::red);
        file.close();
        return;
    }

    for(int i{1};i<array.count();++i){
        m_CamerasConfig[i-1].CameraOwnName = array.at(i)["Camera_Name"].toString();
        m_CamerasConfig[i-1].CameraServerName = array.at(i)["Camera_ServerName"].toString();
        QString ServerName = findServerByResourceName(m_CamerasConfig[i-1].CameraOwnName.toStdString().c_str(),\
                CameraServerNames);
        if(!ServerName.isEmpty()){
            m_CamerasConfig[i-1].CameraServerName = ServerName;
        }else{
            QString str;
            ui->m_DetectedInfo->setTextColor(Qt::red);
            str.sprintf("相机 %s 不存在...",m_CamerasConfig[i-1].CameraOwnName.toStdString().c_str());
        }
        m_CamerasConfig[i-1].CameraConfigFile = array.at(i)["Camera_ConfigFile"].toString();
        m_CamerasConfig[i-1].CameraFlatfieldFile = array.at(i)["Camera_FlatFile"].toString();
        QString str = array.at(i)["Camera_Vender"].toString() + ": " + m_CamerasConfig[i-1].CameraServerName + "   config file : " + \
                m_CamerasConfig[i-1].CameraConfigFile + "   flatfield file: " + m_CamerasConfig[i-1].CameraFlatfieldFile;
        ui->m_DetectedInfo->setTextColor(Qt::green);
        ui->m_DetectedInfo->append(str);
    }
    file.close();
}



QString CameraDetectDialog::findServerByResourceName(const char* ResourceName,const QStringList& ServerNames)
{
    for(const auto& elem : ServerNames){
        char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
        SapManager::GetResourceName(elem.toStdString().c_str(), SapManager::ResourceAcqDevice, 0, deviceName, sizeof(deviceName));
        if(std::strcmp(ResourceName,deviceName) == 0){
            return elem;
        }
    }
    return QString{};
}




/***********************************************************************************************************************************/

CameraConfigDlg::CameraConfigDlg(QStringList& DetectedNames,QWidget* parent) : QDialog(parent),CameraNames(DetectedNames)
{
    setDisplay();
    connect(m_confirmBtn,&QPushButton::clicked,this,&CameraConfigDlg::accept);
    connect(m_pickConfigFileBtn,SIGNAL(clicked()),this,SLOT(pickConfigFileSlot()));
    connect(m_pickFlatPathBtn,SIGNAL(clicked()),this,SLOT(pickFlatFileSlot()));
}

CameraConfigDlg::~CameraConfigDlg()
{


}

void CameraConfigDlg::setDisplay()
{
    m_ServerName = new QLabel{"ServerName:"};
    m_configFile = new QLabel{"ConfigFilePath:"};
    FlatFieldFile = new QLabel{"FlatFieldPath:"};

    m_configPathEdit = new QLineEdit{};
    m_FlatFieldEdit = new QLineEdit{};

    m_pickConfigFileBtn = new QPushButton{"pick..."};
    m_pickFlatPathBtn = new QPushButton{"pick..."};
    m_confirmBtn = new QPushButton{"Apply"};
    m_comboBox = new QComboBox{};
    m_comboBox->addItems(CameraNames);
    mainLayout = new QGridLayout{};

    mainLayout->addWidget(m_ServerName,0,0);
    mainLayout->addWidget(m_comboBox,0,1);
    mainLayout->addWidget(m_configFile,1,0);
    mainLayout->addWidget(m_configPathEdit,1,1);
    mainLayout->addWidget(m_pickConfigFileBtn,1,2);
    mainLayout->addWidget(FlatFieldFile,2,0);
    mainLayout->addWidget(m_FlatFieldEdit,2,1);
    mainLayout->addWidget(m_pickFlatPathBtn,2,2);
    mainLayout->addWidget(m_confirmBtn,3,2,Qt::AlignLeft);

    setLayout(mainLayout);
}

void CameraConfigDlg::updateComboBox()
{
    m_comboBox->clear();
    m_comboBox->addItems(CameraNames);
}


void CameraConfigDlg::pickConfigFileSlot()
{
    QString ccfPath = QFileDialog::getOpenFileName();
    if(!ccfPath.isEmpty())
        m_configPathEdit->setText(ccfPath);
}

void CameraConfigDlg::pickFlatFileSlot()
{
    QString ffPath = QFileDialog::getOpenFileName();
    if(!ffPath.isEmpty())
        m_FlatFieldEdit->setText(ffPath);
}
































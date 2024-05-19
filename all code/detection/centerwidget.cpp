#include "centerwidget.h"

QVector<QString> CenterWidget::CameraVenders;
CenterWidget* CenterWidget::selfPtr{nullptr};   //ָ��������,����ص�����������������

CenterWidget::CenterWidget(QWidget *parent) : QWidget(parent)
{
    SetOverAllUpDisPlay();
    SetUpDisplayDock();
    SetUpInceptionResDock();
    SetUpsysInfoDock();
    SetUpSubWindow();
    CameraVenders<<"CameraA"<<"CameraB"<<"CameraC"<<"CameraD"<<"CameraE"<<"CameraF"<<"CameraG"<<"CameraH";
    selfPtr = this;
    InitialLizeImageState();
    RegisterCamManagerCallBack();
}

CenterWidget::~CenterWidget()
{
    FreeAllCameras();
    DestroyGroupProcessingThread();
    if(m_ShowImage != nullptr){
        delete m_ShowImage;
        m_ShowImage = nullptr;
    }
    for(auto& elem : m_seqImages)
        delete elem;
    m_seqImages.clear();
    SapManager::UnregisterServerCallback();
    SapManager::Close();
}

void CenterWidget::InitialLizeImageState()
{
    for(int i{};i<CameraVenders.size();++i){
        ImageState[CameraVenders.at(i)] = false;
        CamImageRecord[CameraVenders.at(i)] = 0;
        ImageSources[CameraVenders.at(i)] = nullptr;
    }
}

void CenterWidget::SetImageState(QString vender)
{
    ImageState[vender] = true;
}

void CenterWidget::IncreaseCamImgNums(QString vender)
{
    CamImageRecord[vender] += 1;
}

void CenterWidget::SetImageSource(QString vender,void* dataPtr)
{
    ImageSources[vender]->data = (uchar*)(dataPtr); //mode 1
}


void CenterWidget::SetOverAllUpDisPlay()
{
    mg_splitterMain = new QSplitter{Qt::Horizontal,this};
    mg_leftSplitter = new QSplitter{Qt::Vertical};
    //mg_splitterMain->setContentsMargins(0,0,0,0);

    mg_DisplayDock = new QDockWidget{};
    mg_inceptionResDock = new QDockWidget{};
    mg_leftBtmDockWidget = new QDockWidget{};
    mg_DefectsResDock = new QDockWidget{};
    SetDockPriporty();

    mg_leftSplitter->addWidget(mg_inceptionResDock);
    mg_leftSplitter->addWidget(mg_DefectsResDock);
    mg_leftSplitter->addWidget(mg_leftBtmDockWidget);
    mg_splitterMain->addWidget(mg_leftSplitter);
    mg_splitterMain->addWidget(mg_DisplayDock);


    mg_splitterMain->setStretchFactor(0,10);
    mg_splitterMain->setStretchFactor(1,2);
    mainLayout = new QHBoxLayout{};
    mainLayout->addWidget(mg_splitterMain);
    setLayout(mainLayout);
}

void CenterWidget::SetDockPriporty()
{
    mg_DisplayDock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
    mg_inceptionResDock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
    mg_leftBtmDockWidget->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
    mg_DefectsResDock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
    mg_DisplayDock->setWindowTitle("ͼ����ʾ");

    //mg_DisplayDock->setContentsMargins(0,0,0,0);
    mg_inceptionResDock->setWindowTitle("�����Լ����");
    //mg_inceptionResDock->setStyle(new iconned_dock_style( QIcon(":/icons/measuring-tape.png"), mg_inceptionResDock->style()));
    mg_DefectsResDock->setWindowTitle("ȱ�ݼ����");
    //mg_DefectsResDock->setWindowIcon(QIcon{":/icons/defects.png"});
    mg_leftBtmDockWidget->setWindowTitle("ϵͳ��Ϣ");
    //mg_leftBtmDockWidget->setWindowIcon(QIcon{":/icons/information.png"});
}


void CenterWidget::SetUpDisplayDock()
{
    m_basicItem = new BasicItem{m_ShowImage};
    m_basicItem->setPos(QPointF{0.,0.});
    mg_imageScene = new QGraphicsScene{};
    mg_imageScene->addItem(m_basicItem);
    mg_imageViewer = new QGraphicsView{};
    mg_imageViewer->setScene(mg_imageScene);
    mg_DisplayDock->setWidget(mg_imageViewer);
}

void CenterWidget::SetUpInceptionResDock()
{
    m_dataModel = new InceptionResultModel{};
    mg_inceptionDataViewer = new QTableView{};
    mg_inceptionDataViewer->setModel(m_dataModel);
    mg_inceptionResDock->setWidget(mg_inceptionDataViewer);
}

//m_syslogger
void CenterWidget::SetUpsysInfoDock()
{
    m_syslogger = new QTextEdit{};
    mg_leftBtmDockWidget->setWidget(m_syslogger);

}

void CenterWidget::AppendLoggerContent(QString contents,QColor color)
{
    QFont font{"Arial",14};
    m_syslogger->setFont(font);
    m_syslogger->setTextColor(color);
    m_syslogger->append(contents);
}

void CenterWidget::SetUpSubWindow()
{
    mg_CameraDetecter = new CameraDetectDialog{this};
    connect(mg_CameraDetecter,SIGNAL(sendToLogger(QString,QColor)),this,SLOT(AppendLoggerContent(QString,QColor)));
    //sendCameraConfigs(int,CameraConfigs*)
    connect(mg_CameraDetecter,SIGNAL(sendCameraConfigs(int,CameraConfigs*)),this,SLOT(receiveCameraConfigsSlot(int,CameraConfigs*)));


    m_measureStingDia = new MeasureMentSettingDialog{this};
    mg_grascalTunerDia = new GrascaleTunerDia{this};
    mg_sigTunerDia = new SignalTuner{this};
    connect(mg_sigTunerDia,SIGNAL(sendToLogger(QString,QColor)),this,SLOT(AppendLoggerContent(QString,QColor)));
}

void CenterWidget::CheckGroupOneReady(QString vender)
{
    static tp refTimePt = std::chrono::high_resolution_clock::now();
    if(vender != "CameraA" || m_groupOneNums == 0)  //��ֹ�ظ�ͬʱ����
        return;
    int time_consume = 0;
    tp start = std::chrono::high_resolution_clock::now();
    if((start-refTimePt).count() < 20000000)   //20ms�ڲ��ٽ����κβɼ������Ӧ,Ҳ��Ӧ�����κ���Ӧ,�ڴ濽������ʱ��̫����
        return;
    tp end = std::chrono::high_resolution_clock::now();
    time_consume = (end - start).count();
    bool AllAcquired{false};
    while(time_consume <= 15000000 && !AllAcquired)  //�ĸ����������ͼ�������15ms
    {
        int valid_nums{};
        for(int i{0};i<m_groupOneNums;++i){
            if(ImageState[CameraVenders.at(i)]){
                ++valid_nums;
            }
        }
        AllAcquired = valid_nums == 4;
        end = std::chrono::high_resolution_clock::now();
        time_consume = (end - start).count();
    }

    if(AllAcquired){
        //Group one�߳̿�ʼ����
        //SaveImage
        //���õ�ǰ���ڵڼ���Сͼ��
        //��⵱ǰ�ɼ���ͼ�������Ƿ����
        /*
        long base_num = CamImageRecord["CameraA"];
        for( int i{1};i<m_groupOneNums;++i){
            if(CamImageRecord[CameraVenders.at(i)] != base_num){
                qDebug()<<"---------------------------------------------------------------------";
                for(int j{};j<m_groupOneNums;++j){
                    qDebug()<<CameraVenders.at(j)<<" Image nums: "<< CamImageRecord[CameraVenders.at(i)];
                }
                qDebug()<<"---------------------------------------------------------------------";
                break;
                }
            }
          */
        for(int i{};i<m_groupOneNums;++i){
            ImageState[CameraVenders.at(i)] = false;
        }
        ++m_SeqprocessedNums;
        StartGroupOneThread();
    }
    refTimePt = std::chrono::high_resolution_clock::now();  //��¼������ɵ�ʱ��
}

void CenterWidget::CheckGroupTwoReady()
{
    if(m_groupTwoNums == 0)
        return;
    int valid_nums{};
    for(int i{m_groupOneNums};i<m_camCount;++i){
        if(ImageState[CameraVenders.at(i)]){
            ++valid_nums;
        }
    }
    if(valid_nums == m_groupTwoNums){
        //Group Two�߳̿�ʼ����
        StartGroupTwoThread();
    }
}

void CenterWidget::StartGroupOneThread()
{
    qDebug()<<"invoke group one thread...";
    if(m_IsMainThreadKeepLock)    //m_IsMainThreadKeepLock
        group1_mutex->unlock(); //�߳���������,��ʼ����
    else{
        //error,�������ٶȸ�����ͼ��ɼ����ٶ�
        AppendLoggerContent("[FATAL ERROR] >>>�ɼ��ٶȹ���<<<",Qt::red);
    }
}

void CenterWidget::StartGroupTwoThread()
{
    for(int i{};i<m_groupTwoNums;++i){
        ImageState[CameraVenders.at(i)] = false;
    }
    qDebug()<<"invoke group two thread...";

}


void CenterWidget::ImageReceiveFunction(QString CamVender,void* rawData)
{
    selfPtr->SetImageState(CamVender);
    selfPtr->SetImageSource(CamVender,rawData);
    selfPtr->IncreaseCamImgNums(CamVender);
    selfPtr->CheckGroupOneReady(CamVender);
    //selfPtr->CheckGroupTwoReady();
}

void CenterWidget::DetectCamerasSlot()
{
    mg_CameraDetecter->ClearnLoggerInfo();
    mg_CameraDetecter->SetInitialState();
    mg_CameraDetecter->show();
}

bool CenterWidget::ConnectCamerasSlot()
{
    bool status{true};
    for(int i{};i<m_camCount;++i){
        QString curServerName = m_camCfgs[i].CameraServerName;
        QString curCamccf = m_camCfgs[i].CameraConfigFile;
        QString curCamffFile = m_camCfgs[i].CameraFlatfieldFile;
        CameraBasic* CamInstance = new CameraBasic{};
        bool s = CamInstance->SetCamConnect(curServerName,CameraVenders[i],curCamccf,curCamffFile);
        if(s){
            AppendLoggerContent("[���] " + CameraVenders[i] + " ���ӳɹ�",Qt::green);
            LineScanCameras.push_back(CamInstance);
        }else{
            AppendLoggerContent("[���] " + CameraVenders[i] + " ����ʧ��",Qt::red);
            FreeAllCameras();
            status = false;
            break;
        }
    }
    return status;
}

void CenterWidget::DisConnectCamerasSlot()
{
    FreeAllCameras();
}


void CenterWidget::receiveCameraConfigsSlot(int camCount,CameraConfigs* cfgs)
{
    if(camCount <= 0)
        return;

    bool ISConfigFileValid{true};
    for(int i{};i<camCount;++i){
        if(cfgs[i].CameraServerName == "None"){
            ISConfigFileValid = false;
            AppendLoggerContent("[ϵͳ] ��Ч������,�����ļ����������������ƥ��!!!",Qt::red);
            break;
        }
    }
    if(ISConfigFileValid){
        QString str;
        AppendLoggerContent("[ϵͳ] ���յ�" + str.setNum(camCount) + "���������",Qt::blue);
        m_camCount = camCount;
        if(m_camCount > 4){
            m_groupOneNums = 4;
            m_groupTwoNums = camCount - m_groupOneNums;
        }else{
            m_groupOneNums = m_camCount;
            m_groupTwoNums = 0;
        }
        m_camCfgs = cfgs;
        emit CanConnectCameras();
    }
}

void CenterWidget::SaveTuneImage()
{
    QString str = R"(C:\Users\Y_hua\Desktop\TuneImage)";
    if(!str.isEmpty()){
        QDateTime curTime = QDateTime::currentDateTime();
        QString ImageASaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamA.jpg";
        QString ImageBSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamB.jpg";
        QString ImageCSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamC.jpg";
        QString ImageDSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamD.jpg";
        QString ImageSSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "StichedImage.jpg";
        if(ImageSources["CameraA"] != nullptr){
            //cv::imwrite(ImageASaveName.toStdString(),*ImageSources["CameraA"]);
        }
        if(ImageSources["CameraB"] != nullptr){
            //cv::imwrite(ImageBSaveName.toStdString(),*ImageSources["CameraB"]);
        }
        if(ImageSources["CameraC"] != nullptr){
            //cv::imwrite(ImageCSaveName.toStdString(),*ImageSources["CameraC"]);
        }
        if(ImageSources["CameraD"] != nullptr){
            //cv::imwrite(ImageDSaveName.toStdString(),*ImageSources["CameraD"]);
        }
        if(m_ShowImage != nullptr){
            //QPixmap srcPixmap = QPixmap::fromImage(*m_ShowImage);
            //srcPixmap.save(ImageSSaveName);
            m_ShowImage->save(ImageSSaveName);
        }
    }
}

void CenterWidget::SaveImageSlot()
{
    QString str = QFileDialog::getExistingDirectory();
    if(!str.isEmpty()){
        QDateTime curTime = QDateTime::currentDateTime();
        QString ImageASaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamA.jpg";
        QString ImageBSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamB.jpg";
        QString ImageCSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamC.jpg";
        QString ImageDSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamD.jpg";
        QString ImageSSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "StichedImage.jpg";
        if(ImageSources["CameraA"] != nullptr){
            //cv::imwrite(ImageASaveName.toStdString(),*ImageSources["CameraA"]);
        }
        if(ImageSources["CameraB"] != nullptr){
            //cv::imwrite(ImageBSaveName.toStdString(),*ImageSources["CameraB"]);
        }
        if(ImageSources["CameraC"] != nullptr){
            //cv::imwrite(ImageCSaveName.toStdString(),*ImageSources["CameraC"]);
        }
        if(ImageSources["CameraD"] != nullptr){
            //cv::imwrite(ImageDSaveName.toStdString(),*ImageSources["CameraD"]);
        }
        if(m_ShowImage != nullptr){
            //QPixmap srcPixmap = QPixmap::fromImage(*m_ShowImage);
            //srcPixmap.save(ImageSSaveName);
            m_ShowImage->save(ImageSSaveName);
        }
    }

}


void CenterWidget::LockSplitterPartition()
{
    mg_splitterMain->setCollapsible(0,true);
    mg_splitterMain->setCollapsible(1,true);
    mg_leftSplitter->setCollapsible(0,true);
    mg_leftSplitter->setCollapsible(1,true);
    mg_leftSplitter->setCollapsible(2,true);
}

void CenterWidget::UnLockSplitterPartition()
{
    mg_splitterMain->setCollapsible(0,false);
    mg_splitterMain->setCollapsible(1,false);
    mg_leftSplitter->setCollapsible(0,false);
    mg_leftSplitter->setCollapsible(1,false);
    mg_leftSplitter->setCollapsible(2,false);
}

void CenterWidget::FreeAllCameras()
{
    for(auto& elem : LineScanCameras){  //LineScanCameras��һ������Ч���
        QString str;
        str = "[���] " + elem->GetCameraVender() + " �Ͽ����ӳɹ�";
        AppendLoggerContent(str,Qt::red);
        delete elem;
    }
    LineScanCameras.clear();
    for(int i{};i<CameraVenders.size();++i){    //�ͷ�ͼ���ڴ�
        if(ImageSources[CameraVenders.at(i)] != nullptr){
            delete ImageSources[CameraVenders.at(i)];
            ImageSources[CameraVenders.at(i)] = nullptr;
        }
    }
    DestroyGroupProcessingThread(); //�ͷŴ����߳�
    IsMemoryAllocated = false;
    emit SetSoftWareToinitialState();
}

bool CenterWidget::IsCamExternTriggerOn()
{
    auto tmpCamera = LineScanCameras.at(0);
    return tmpCamera->IsExternalTriggerOn();
}

void CenterWidget::AllocateCameraMemory()
{
    qDebug()<<"AllocateCameraMemory";
    if(!IsMemoryAllocated){
        int CamReadyNums{0};
        bool IsSetCallReady{false};
        for(auto& elem : LineScanCameras){
            qDebug()<<"..............................";
            //���ûص�����
            if(!IsSetCallReady){
                elem->SetHostCallBack(ImageReceiveFunction);
                IsSetCallReady = true;
            }
            bool s = elem->SetCamReadyToSnap();
            if(s){
                ImageSize res = elem->getImageSize();
                cv::Mat* curImage = new cv::Mat(res.height,res.width,CV_8U);
                ImageSources[elem->GetCameraVender()] = curImage;
                QString str;
                str ="[���] " + elem->GetCameraVender() + " ����������ɹ�...";
                AppendLoggerContent(str,Qt::green);
            }else{
                QString str ="[���] " + elem->GetCameraVender() + " ����������ʧ��...";
                AppendLoggerContent(str,Qt::red);
                FreeAllCameras();   //���¿�ʼ����,�ɼ�
                break;
            }
            ++CamReadyNums;
        }
        if(CamReadyNums == m_camCount){
            IsMemoryAllocated = true;
            CreatGroupProcessingThread();
            QString str ="[ϵͳ] ͼ�����̴߳����ɹ�";
            AppendLoggerContent(str,Qt::green);
        }
    }
}

void CenterWidget::StartSeqInception()
{
    //qDebug()<<"Start Sequntial Inception...";
    //�������������Ƿ����Ҫ��
    if(CheckSequenInceptionStting()){
        //ֱ��Grab???;
        for(auto& elem : LineScanCameras){
            if(elem->StartGrab()){
            QString str ="[���] " + elem->GetCameraVender() + " ��ʼ�ɼ�...";
            AppendLoggerContent(str,Qt::green);
        }
        }
    }else{
        AppendLoggerContent("��������ļ������ϼ��Ҫ��",Qt::red);
    }
}

void CenterWidget::StopSeqInception()
{
    for(auto& elem : LineScanCameras){
        bool s = elem->StopGrab();
        if(s){
            QString str ="[���] " + elem->GetCameraVender() + " ֹͣ�ɼ��ɹ�...";
            //�����������͵�ǰ���������
            elem->ClearnBuffer();
            AppendLoggerContent(str,Qt::red);
        }else{
            QString str ="[���] " + elem->GetCameraVender() + " ֹͣ�ɼ�ʧ��!!!...";
            AppendLoggerContent(str,Qt::red);
            FreeAllCameras();
            break;
        }
    }
    ResetImageIndex();  //���¼���,ǰһ֡����Ĳ��ֶ���
}

void CenterWidget::StartInception()
{
    //���������ʼSnap
    for(auto& elem : LineScanCameras){
        if(elem->StartSnap()){
        QString str ="[���] " + elem->GetCameraVender() + " ��ʼ�ɼ�...";
        AppendLoggerContent(str,Qt::green);
    }
    }
}

void CenterWidget::StopInception()
{
    for(auto& elem : LineScanCameras){
        bool s = elem->StopSnap();
        if(s){
            QString str ="[���] " + elem->GetCameraVender() + " ֹͣ�ɼ��ɹ�...";
            AppendLoggerContent(str,Qt::red);
        }else{
            QString str ="[���] " + elem->GetCameraVender() + " ֹͣ�ɼ�ʧ��!!!...";
            AppendLoggerContent(str,Qt::red);
            FreeAllCameras();
            break;
        }
    }
}

void CenterWidget::StartGrab()   //��ʼ�����ɼ�
{
    for(auto& elem : LineScanCameras){
        if(elem->StartGrab()){
        QString str ="[���] " + elem->GetCameraVender() + " ��ʼ�����ɼ�...";
        AppendLoggerContent(str,Qt::green);
    }else{
            QString str ="[���] " + elem->GetCameraVender() + " �����ɼ�ʧ��...";
            AppendLoggerContent(str,Qt::red);
        }
    }
}

void CenterWidget::StopGrab()    //ֹͣ�����ɼ�
{
    for(auto& elem : LineScanCameras){
        bool s = elem->StopGrab();
        if(s){
            QString str ="[���] " + elem->GetCameraVender() + " ֹͣ�ɼ��ɹ�...";
            AppendLoggerContent(str,Qt::red);
        }else{
            QString str ="[���] " + elem->GetCameraVender() + " ֹͣ�ɼ�ʧ��!!!...";
            AppendLoggerContent(str,Qt::red);
            FreeAllCameras();
            break;
        }
    }
}


void CenterWidget::CreatGroupProcessingThread()
{
    group1_mutex = new QMutex{};
    group1_mutex->lock();
    m_IsMainThreadKeepLock = true;
    qDebug()<<"mutex created....";
    m_GroupOnePThread = new ImageProcessThread{m_ShowImage,ImageSources,group1_mutex,\
            m_groupOneNums,m_IsMainThreadKeepLock,m_separationTotalImage,m_seqImages,m_IsOnSepInception,m_SeqprocessedNums};
    //HandleMutexStaticFunction
    m_GroupOnePThread->setMutexCallBack(HandleMutexStaticFunction); //�����߳����ص�����
    connect(m_GroupOnePThread,SIGNAL(ShowImageReady()),this,SLOT(groupOneImageAcquiredSlot()));
    //connect(m_GroupOnePThread,SIGNAL(MutexUnlocked()),this,SLOT(lockgroupOneMutexSlot()));

    connect(m_GroupOnePThread,SIGNAL(ImageProcessDone()),this,SLOT(ShowImageProcessingResultSlot()));
    m_GroupOnePThread->start();
    if(m_dataModel != nullptr)
        m_GroupOnePThread->SetDataModel(m_dataModel);
    //m_GroupOnePThread->start();
}

void CenterWidget::DestroyGroupProcessingThread()
{
    if(m_GroupOnePThread != nullptr){
       if(m_IsMainThreadKeepLock){   //���߳�ӵ����,���߳�Ŀǰ����
           m_GroupOnePThread->StopTheThread();  //����ѭ��,�������߳�
            group1_mutex->unlock(); //�ͷ���,�����߳�����,�Զ����е�break,�ͷ���
            m_IsMainThreadKeepLock = false;

       Sleep(100);
       m_GroupOnePThread->quit();
       m_GroupOnePThread->wait(100);
       delete m_GroupOnePThread;
       m_GroupOnePThread = nullptr;
       delete group1_mutex;
       group1_mutex = nullptr;
       }else{//���߳�ӵ����(���ڴ���ͼ��)
           Sleep(1000);  //�ȴ�1�������߳��ͷ���
           if(m_IsMainThreadKeepLock){   //���߳�ӵ����,���߳�Ŀǰ����
               m_GroupOnePThread->StopTheThread();  //����ѭ��,�������߳�
                group1_mutex->unlock(); //�ͷ���,�����߳�����,
                m_IsMainThreadKeepLock = false;

           Sleep(100);
           m_GroupOnePThread->quit();
           m_GroupOnePThread->wait(100);
           delete m_GroupOnePThread;
           m_GroupOnePThread = nullptr;
           delete group1_mutex;
           group1_mutex = nullptr;
           }else{
               qDebug()<<"release lock Fail....";
           }
       }

    }
}

void CenterWidget::groupOneImageAcquiredSlot()
{
    //��ʾͼƬ
    m_basicItem->update();

}

void CenterWidget::lockMutexLocker()
{
    qDebug()<<"call lockMutexLocker....";
    if(group1_mutex != nullptr)
    {
        group1_mutex->lock();
        m_IsMainThreadKeepLock = true;
    }
}

void CenterWidget::HandleMutexStaticFunction()
{
    selfPtr->lockMutexLocker();
}

/*
void CenterWidget::lockgroupOneMutexSlot()
{
    if(group1_mutex != nullptr){
        group1_mutex->lock();
        m_IsMainThreadKeepLock = true;
    }
}
*/

void CenterWidget::ShowImageProcessingResultSlot()
{
    mg_inceptionDataViewer->viewport()->update();
    m_basicItem->update();
}

void CenterWidget::groupTwoImageAcquiredSlot()
{


}


void CenterWidget::ServerCallback(SapManCallbackInfo *pInfo)
{
    int serverIndex = pInfo->GetServerIndex();
    char serverName[64];
    char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
    SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
    SapManager::GetResourceName(serverIndex,SapManager::ResourceAcqDevice,0,deviceName);
    QString str;
    switch (pInfo->GetEventType())
    {
       case SapManager::EventServerNew:
          //printf("\n==> Camera %s was connected for the first time\n", serverName);
          str.sprintf("[���] ��� %s �״�����...",deviceName);
          selfPtr->AppendLoggerContent(str,Qt::blue);
          break;

       case SapManager::EventServerDisconnected:
          str.sprintf("[���] ��� %s ����...",deviceName);
          selfPtr->AppendLoggerContent(str,Qt::red);
          break;

       case SapManager::EventServerConnected:
          str.sprintf("[���] ��� %s ��������...",deviceName);
          selfPtr->AppendLoggerContent(str,Qt::green);
          break;
    }

}

void CenterWidget::RegisterCamManagerCallBack()
{
    SapManager::Open();
    SapManager::RegisterServerCallback(SapManager::EventServerNew | SapManager::EventServerDisconnected | \
                                       SapManager::EventServerConnected | SapManager::EventResourceInfoChanged, ServerCallback, NULL);
}

void CenterWidget::MeasureMentSettingSlot(){  //�����ص��Ͳ�������
    //QVector<int> GetStichParameters()
    if(m_GroupOnePThread == nullptr)
        return;
    QVector<int> vals = m_GroupOnePThread->GetStichParameters();
    m_measureStingDia->SetGroup1ValidIndex(vals);
    if(m_measureStingDia->exec()) {
        QVector<int> G1IDX = m_measureStingDia->GetGroup1ValidIndex();
        m_GroupOnePThread->SetStichParameters(G1IDX);
        //qDebug()<<G1IDX;
        //save current settings-------------------------------------------------------------------------------------------------------------------------
        QJsonArray cur_array;
        for(int i{};i<G1IDX.size();++i){
            QJsonObject cur_obj;
            cur_obj.insert("OverLap",G1IDX.at(i));
            cur_array.append(cur_obj);
        }
        QFile file{R"(./OverLap.json)"};
        if(!file.open(QIODevice::ReadWrite | QFile::Truncate))
            qDebug()<<"open local error...";
        else{
            QJsonDocument cur_doc;
            cur_doc.setArray(cur_array);
            file.write(cur_doc.toJson());
            file.close();
        }

    }
}

void CenterWidget::ColorBalanceSlot()
{
    //qDebug()<<"ColorBalanceSlot";
    QStringList FFtPaths;
    QVector<double> Gains;
    for(int i{};i<8;++i){
        FFtPaths.push_back("None");
        Gains.push_back(-1.);
    }
    for(int i{};i<LineScanCameras.size();++i){
        FFtPaths[i] = LineScanCameras[i]->GetFFtPath();
        double gain;
        if(LineScanCameras[i]->GetFeatureVal("Gain",gain))
            //qDebug()<<"gain : "<<gain;
            Gains[i] = gain;
    }
    mg_grascalTunerDia->SetInitialGainState(Gains,1);
    mg_grascalTunerDia->SetInitialGainState(Gains,2);
    mg_grascalTunerDia->SetInitialPathState(FFtPaths,1);
    mg_grascalTunerDia->SetInitialPathState(FFtPaths,2);
    if(mg_grascalTunerDia->exec()){
        mg_grascalTunerDia->ApplyG1SettingsSlot();
        mg_grascalTunerDia->ApplyG2SettingsSlot();
        if(mg_grascalTunerDia->G1Ok && m_camCount > 0){
            QStringList G1FFtPaths = mg_grascalTunerDia->GetFFtPathsSettings(1);
            QVector<double> G1Gains= mg_grascalTunerDia->GetGainValsSettings(1);
            for(int i{};i<LineScanCameras.size();++i){
                LineScanCameras[i]->SetFeatureVal("Gain",G1Gains[i]);
                LineScanCameras[i]->SetValidPath(G1FFtPaths[i],"FFT");
            }
            mg_grascalTunerDia->G1Ok = false;
            AppendLoggerContent("[ϵͳ] GroupOne ������ø��³ɹ�...",Qt::green);
        }
        if(mg_grascalTunerDia->G2OK && m_camCount > 4){
            QStringList G2FFtPaths = mg_grascalTunerDia->GetFFtPathsSettings(2);
            QVector<double> G2Gains = mg_grascalTunerDia->GetGainValsSettings(2);
            for(int i{4};i<LineScanCameras.size();++i){
                LineScanCameras[i]->SetFeatureVal("Gain",G2Gains[i]);
                LineScanCameras[i]->SetValidPath(G2FFtPaths[i],"FFT");
            }
            mg_grascalTunerDia->G2OK = false;
            AppendLoggerContent("[ϵͳ] GroupTwo ������ø��³ɹ�...",Qt::green);
        }
    }
}


void CenterWidget::SignalTriggerSlot()
{
    mg_sigTunerDia->SetCameras(LineScanCameras);
    mg_sigTunerDia->show();
}


void CenterWidget::LoadExitCalibRes()
{
    if(m_GroupOnePThread != nullptr){
        bool res = m_GroupOnePThread->LoadExitCalibRes();
        if(res){
            AppendLoggerContent("[ϵͳ] GroupOne ����궨�������سɹ�...",Qt::green);
        }else{
            AppendLoggerContent("[ϵͳ] GroupOne ����궨��������ʧ��...",Qt::red);
        }
    }else{
        AppendLoggerContent("[ϵͳ] GroupOne ����궨��������ʧ��(�߳�δ����)...",Qt::red);
    }
}

/*****************************************�����ɼ��������*********************************************************/
bool CenterWidget::CheckSequenInceptionStting()  //���ڼ���������ɼ��������Ƿ���Ϲ淶
{
    //����Ƿ�Ϊ�ⴥ��,�Ҳɼ��ĺ�������Ҫ��(2000~3000)
    bool Ck_res{true};
    int imageHeight{};
    for(const auto& elem : LineScanCameras){
        if(!elem->IsExternalTriggerOn()){
            Ck_res = false;
            break;
        }
        elem->GetFeatureVal("Height",imageHeight);
        if(!(500 <= imageHeight && imageHeight < 3000)){
            Ck_res = false;
            break;
        }
    }
    return Ck_res;
}

void CenterWidget::SWitchInceptionMode(QString mode)
{
    if(mode == "Single Inception"){
        m_IsOnSepInception = false;
        AppendLoggerContent("[ϵͳ] >>>�л���mode 1<<<",Qt::green);
    }else if(mode == "Squential Inception")
    {
        m_IsOnSepInception = true;
        AppendLoggerContent("[ϵͳ] >>>�л���mode 2<<<",Qt::green);
        m_GroupOnePThread->SepIncepReConsuctions();
    }

}

void CenterWidget::ResetImageIndex()
{
    m_SeqprocessedNums = 0;
}









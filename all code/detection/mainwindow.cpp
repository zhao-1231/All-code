#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QString("�������"));
    SetUpDisPlay();
    SetCamControlMenu();
    SetUpConnections();
    SetUpInceptionDialog();
    //setContentsMargins(0,0,0,0);
}

MainWindow::~MainWindow()
{

}


void MainWindow::SetUpDisPlay()
{
    mg_centerWidget = new CenterWidget{};
    setCentralWidget(mg_centerWidget);
    connect(mg_centerWidget,SIGNAL(CanConnectCameras()),this,SLOT(EnableCamConnectAction()));
    connect(mg_centerWidget,SIGNAL(SetSoftWareToinitialState()),this,SLOT(SetSoftWareToinitialStateSlot()));
}

void MainWindow::SetUpInceptionDialog()
{
    mg_incepDia = new DefectInceptionDialog{this};
}

void MainWindow::SetCamControlMenu()
{
    m_CamControlMenu = new QMenu{"�������"};
    m_UIControlMenu = new QMenu{"������ʾ����"};
    m_InCeptionMenu = new QMenu{"������"};
    m_InCeptionMenu->setDisabled(true);

    m_CamDetectionAct = new QAction{"�����������"};
    m_CamDetectionAct->setIcon(QIcon{":/icons/settings.png"});
    m_CamConnectAct = new QAction{"�������"};
    m_CamConnectAct->setDisabled(true);
    m_CamConnectAct->setIcon(QIcon{":/icons/link.png"});
    m_CamDisConnectAct = new QAction{"�Ͽ����"};
    m_CamDisConnectAct->setDisabled(true);
    m_CamDisConnectAct->setIcon(QIcon{":/icons/breaklink.png"});

    m_lockCurPatitionAct = new QAction{"�������ط���"};
    m_lockCurPatitionAct->setIcon(QIcon{":/icons/screenLock.png"});
    m_unlockCurPatitionAct = new QAction{"��ֹ���ط���"};
    m_unlockCurPatitionAct->setIcon(QIcon{":/icons/screenUnLock.png"});
    m_UIControlMenu->addAction(m_lockCurPatitionAct);
    m_UIControlMenu->addAction(m_unlockCurPatitionAct);

    m_AllocateCamMemsAct = new QAction{"�����������"};
    m_AllocateCamMemsAct->setIcon(QIcon{":/icons/ram.png"});
    m_AllocateCamMemsAct->setDisabled(true);
    m_StartSequnenceInception = new QAction{"�������(test)"};
    m_StopSequnenceInception = new QAction{"ֹͣ�������(test)"};
    m_StartSequnenceInception->setDisabled(true);
    m_StopSequnenceInception->setDisabled(true);
    m_StartInceptionAct = new QAction{"��ʼ���"};
    m_StartInceptionAct->setDisabled(true);
    m_StartInceptionAct->setIcon(QIcon{":/icons/play.png"});
    m_StopInceptionAct = new QAction{"ֹͣ���"};
    m_StopInceptionAct->setDisabled(true);
    m_StopInceptionAct->setIcon(QIcon{":/icons/pause.png"});

    m_StartGrab = new QAction{"�����ɼ�"};
    m_StartGrab->setIcon(QIcon{":/icons/play.png"});
    m_StartGrab->setDisabled(true);
    m_StopGrab = new QAction{"ֹͣ�����ɼ�"};
    m_StopGrab->setIcon(QIcon{":/icons/pause.png"});
    m_StopGrab->setDisabled(true);

    m_measureMentSettingAct = new QAction{"�����Լ������"};
    m_measureMentSettingAct->setIcon(QIcon{":/icons/ruler.png"});
    m_measureMentSettingAct->setDisabled(true);
    m_defetsParamSettingAct = new QAction{"ȱ�ݼ������"};
    m_defetsParamSettingAct->setIcon(QIcon{":/icons/eye.png"});
    m_defetsParamSettingAct->setDisabled(true);
    m_FunctionSettingMenu = new QMenu{"����"};
    m_FunctionSettingMenu->setIcon(QIcon{":/icons/setting1.png"});
    m_FunctionSettingMenu->setDisabled(true);
    m_ColorBalanceAct = new QAction{"�ҶȾ�������"};
    m_ColorBalanceAct->setIcon(QIcon{":/icons/Tuner.png"});
    m_triggerSigAct = new QAction{"�����ź�����"};
    m_triggerSigAct->setIcon(QIcon{":/icons/radio-waves.png"});
    m_EnableFlatMenu = new QMenu{"ƽ������"};
    m_EnableFlatAct = new QAction{"����"};
    m_StopFlatAct = new QAction{"�ر�"};


    m_measureDefectParamsMenu = new QMenu{"������"};
    m_SepCalibParamsAct = new QAction{"�ֶα궨����"};
    m_SepCalibParamsAct->setIcon(QIcon{":/icons/measuring-tape.png"});
    m_defectSettingsAct = new QAction{"ȱ�ݼ�����"};
    m_defectSettingsAct->setIcon(QIcon{":/icons/eye.png"});
    m_measureDefectParamsMenu->addAction(m_SepCalibParamsAct);
    m_measureDefectParamsMenu->addAction(m_defectSettingsAct);


    m_miscMenu = new QMenu{"����"};
    m_SaveImageAct = new QAction{"����ͼ��"};
    m_SaveImageAct->setIcon(QIcon{":/icons/Save.png"});
    m_miscMenu->addAction(m_SaveImageAct);


    //m_AllocateCamMemsAct
    //m_InCeptionMenu->addAction(m_AllocateCamMemsAct);
    m_InCeptionMenu->addAction(m_StartSequnenceInception);
    m_InCeptionMenu->addAction(m_StopSequnenceInception);
    m_InCeptionMenu->addAction(m_StartInceptionAct);
    m_InCeptionMenu->addAction(m_StopInceptionAct);
    m_InCeptionMenu->addAction(m_StartGrab);
    m_InCeptionMenu->addAction(m_StopGrab);
    m_InCeptionMenu->addMenu(m_FunctionSettingMenu);
    m_FunctionSettingMenu->addAction(m_ColorBalanceAct);
    m_FunctionSettingMenu->addAction(m_triggerSigAct);
    m_FunctionSettingMenu->addAction(m_measureMentSettingAct);
    m_FunctionSettingMenu->addAction(m_defetsParamSettingAct);
    m_FunctionSettingMenu->addMenu(m_EnableFlatMenu);
    m_EnableFlatMenu->addAction(m_EnableFlatAct);
    m_EnableFlatMenu->addAction(m_StopFlatAct);
    //m_triggerSigAct

    m_CamControlMenu->addAction(m_CamDetectionAct);
    m_CamControlMenu->addAction(m_CamConnectAct);
    m_CamControlMenu->addAction(m_AllocateCamMemsAct);
    m_CamControlMenu->addAction(m_CamDisConnectAct);
    menuBar()->addMenu(m_CamControlMenu);
    menuBar()->addMenu(m_InCeptionMenu);
    menuBar()->addMenu(m_measureDefectParamsMenu);
    menuBar()->addMenu(m_UIControlMenu);
    menuBar()->addMenu(m_miscMenu);
}

void MainWindow::SetUpConnections()
{
    connect(m_CamDetectionAct,SIGNAL(triggered()),this,SLOT(DetectCamerasSlot()));
    //m_CamConnectAct
    connect(m_CamConnectAct,SIGNAL(triggered()),this,SLOT(ConnectCamerasSlot()));
    //m_CamDisConnectAct
    connect(m_CamDisConnectAct,SIGNAL(triggered()),this,SLOT(DisConnectCamerasSlot()));
    //m_StartSequnenceInception
    connect(m_StartSequnenceInception,SIGNAL(triggered()),this,SLOT(StartSequnenceInceptionSlot()));
    //m_StopSequnenceInception
    connect(m_StopSequnenceInception,SIGNAL(triggered()),this,SLOT(StopSequnenceInceptionSlot()));
    //m_StartInceptionAct
    connect(m_StartInceptionAct,SIGNAL(triggered()),this,SLOT(StartInceptionSlot()));
    //m_StopInceptionAct
    connect(m_StopInceptionAct,SIGNAL(triggered()),this,SLOT(StopInceptionSlot()));
    //m_StopInceptionAct
    connect(m_StartGrab,SIGNAL(triggered()),this,SLOT(StartGrabSlot()));
    //m_StopInceptionAct
    connect(m_StopGrab,SIGNAL(triggered()),this,SLOT(StopGrabSlot()));
    //m_AllocateCamMemsAct

    connect(m_AllocateCamMemsAct,SIGNAL(triggered()),this,SLOT(AllocateCamMemsSlot()));
    //m_SepCalibParamsAct
    connect(m_SepCalibParamsAct,SIGNAL(triggered()),this,SLOT(LoadExittingCalibResultSlot()));

    connect(m_lockCurPatitionAct,SIGNAL(triggered()),mg_centerWidget,SLOT(LockSplitterPartition()));
    connect(m_unlockCurPatitionAct,SIGNAL(triggered()),mg_centerWidget,SLOT(UnLockSplitterPartition()));
    //m_SaveImageAct
    connect(m_SaveImageAct,SIGNAL(triggered()),mg_centerWidget,SLOT(SaveImageSlot()));
    connect(m_measureMentSettingAct,SIGNAL(triggered()),mg_centerWidget,SLOT(MeasureMentSettingSlot()));
    //m_ColorBalanceAct
    connect(m_ColorBalanceAct,SIGNAL(triggered()),mg_centerWidget,SLOT(ColorBalanceSlot()));
    //m_triggerSigAct
    connect(m_triggerSigAct,SIGNAL(triggered()),mg_centerWidget,SLOT(SignalTriggerSlot()));
    //m_defetsParamSettingAct
    connect(m_defetsParamSettingAct,SIGNAL(triggered()),this,SLOT(ShowDefectIncepDialogSlot()));
}


void MainWindow::EnableCamConnectAction()
{
    m_CamConnectAct->setEnabled(true);
}

void MainWindow::DetectCamerasSlot()
{
    mg_centerWidget->DetectCamerasSlot();
}

void MainWindow::ConnectCamerasSlot()
{
    bool s = mg_centerWidget->ConnectCamerasSlot();
    if(s){
        m_CamDetectionAct->setDisabled(true);
        m_CamConnectAct->setDisabled(true);
        m_CamDisConnectAct->setEnabled(true);
        m_AllocateCamMemsAct->setEnabled(true);
    }
}

void MainWindow::DisConnectCamerasSlot()
{
    mg_centerWidget->DisConnectCamerasSlot();
    //SetSoftWareToinitialStateSlot();
    //m_CamConnectAct->setEnabled(true);
}

void MainWindow::StartSequnenceInceptionSlot() //�߲ɼ��߼��ģʽ
{
    //�������
    mg_centerWidget->StartSeqInception();
}

void MainWindow::StopSequnenceInceptionSlot()  //�߲ɼ��߼��ģʽ
{
    mg_centerWidget->StopSeqInception();
}

void MainWindow::StartInceptionSlot()
{
    mg_centerWidget->StartInception();
}

void MainWindow::StopInceptionSlot()
{
    mg_centerWidget->StopInception();
}

void MainWindow::StartGrabSlot()
{
    mg_centerWidget->StartGrab();
}

void MainWindow::StopGrabSlot()
{
    mg_centerWidget->StopGrab();
}

void MainWindow::AllocateCamMemsSlot()
{
    mg_centerWidget->AllocateCameraMemory();

    if(mg_centerWidget->IsCamExternTriggerOn()){
        //qDebug()<<""
        m_StartInceptionAct->setEnabled(true);
        m_StopInceptionAct->setEnabled(true);
    }else{
        m_StartGrab->setEnabled(true);
        m_StopGrab->setEnabled(true);
    }

    m_measureMentSettingAct->setEnabled(true);
    m_defetsParamSettingAct->setEnabled(true);
    m_FunctionSettingMenu->setEnabled(true);
    m_InCeptionMenu->setEnabled(true);
}


void MainWindow::SetSoftWareToinitialStateSlot()
{
    m_CamDetectionAct->setEnabled(true);
    m_CamConnectAct->setDisabled(true);
    m_AllocateCamMemsAct->setDisabled(true);
    m_CamDisConnectAct->setDisabled(true);
    m_InCeptionMenu->setDisabled(true);
    m_AllocateCamMemsAct->setEnabled(true);
    m_StartInceptionAct->setDisabled(true);
    m_StopInceptionAct->setDisabled(true);
    m_FunctionSettingMenu->setDisabled(true);
    m_measureMentSettingAct->setDisabled(true);
    m_defetsParamSettingAct->setDisabled(true);
    m_AllocateCamMemsAct->setDisabled(true);
}

void MainWindow::LoadExittingCalibResultSlot()
{
    mg_centerWidget->LoadExitCalibRes();
}

void MainWindow::ShowDefectIncepDialogSlot()
{
    if(mg_incepDia->exec()){
        QString mode = mg_incepDia->GetInceptionMode();
        mg_centerWidget->SWitchInceptionMode(mode);
        if(mode == "Single Inception"){
            qDebug()<<"Single Inception";
            m_StartInceptionAct->setEnabled(true);
            m_StopInceptionAct->setEnabled(true);
            m_StartSequnenceInception->setDisabled(true);
            m_StopSequnenceInception->setDisabled(true);
        }else if(mode == "Squential Inception"){
            qDebug()<<"Squential Inception";
            m_StartInceptionAct->setDisabled(true);
            m_StopInceptionAct->setDisabled(true);
            m_StartSequnenceInception->setEnabled(true);
            m_StopSequnenceInception->setEnabled(true);
        }
    }
}


















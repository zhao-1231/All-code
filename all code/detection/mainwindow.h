#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include "centerwidget.h"
#include "defectinceptiondialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void SetUpDisPlay();    //����������ʾ
    void SetCamControlMenu();   //�˵�������ʾ
    void SetUpConnections();    //�˵�������
    void SetUpInceptionDialog();
private:
    //Menus and Actions
    QMenu* m_CamControlMenu{};
    QAction* m_CamDetectionAct{};
    QAction* m_CamConnectAct{};
    QAction* m_CamDisConnectAct{};
    /***********************************/
    QMenu* m_UIControlMenu{};
    QAction* m_lockCurPatitionAct{};
    QAction* m_unlockCurPatitionAct{};
    /*******************************************/
    QMenu* m_InCeptionMenu{};
    QAction* m_AllocateCamMemsAct{};
    QAction* m_StartSequnenceInception{};    //�߲ɼ��߼��
    QAction* m_StopSequnenceInception{};
    QAction* m_StartInceptionAct{};
    QAction* m_StopInceptionAct{};
    QAction* m_StartGrab{}; //�����ɼ�,����ʵʱ���ڹ�Դ
    QAction* m_StopGrab{};
    QMenu* m_FunctionSettingMenu{};
    QAction* m_measureMentSettingAct{};
    QAction* m_defetsParamSettingAct{};
    QAction* m_ColorBalanceAct{};
    QAction* m_triggerSigAct{};
    QMenu* m_EnableFlatMenu{};
    QAction* m_EnableFlatAct{};
    QAction* m_StopFlatAct{};
    /******************************************************************/
    QMenu* m_measureDefectParamsMenu{};
    QAction* m_SepCalibParamsAct{};
    QAction* m_defectSettingsAct{};
    /******************************************************************/
    QMenu* m_miscMenu{};
    QAction* m_SaveImageAct{};
private:
    //CenterWidgets
    CenterWidget* mg_centerWidget{};
    //inception Dialog
    DefectInceptionDialog* mg_incepDia{};
private slots:
    void DetectCamerasSlot();
    void ConnectCamerasSlot();
    void DisConnectCamerasSlot();
    void EnableCamConnectAction();
    void StartSequnenceInceptionSlot(); //�߲ɼ��߼��ģʽ
    void StopSequnenceInceptionSlot();  //�߲ɼ��߼��ģʽ
    void StartInceptionSlot();
    void StopInceptionSlot();
    void StartGrabSlot();
    void StopGrabSlot();
    void AllocateCamMemsSlot();
    void SetSoftWareToinitialStateSlot();
    void LoadExittingCalibResultSlot();
    void ShowDefectIncepDialogSlot();
};
#endif // MAINWINDOW_H

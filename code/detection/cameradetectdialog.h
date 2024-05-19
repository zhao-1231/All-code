#ifndef CAMERADETECTDIALOG_H
#define CAMERADETECTDIALOG_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QLabel>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <SapClassBasic.h>
#pragma execution_character_set("utf-8")
//最多接受8个相机,可以添加

struct CameraConfigs
{
    QString CameraServerName;
    QString CameraOwnName;
    QString CameraConfigFile;
    QString CameraFlatfieldFile;
    CameraConfigs(){CameraServerName = "None";CameraOwnName="None";CameraConfigFile="None";CameraFlatfieldFile="None";}
};

namespace Ui {
class CameraDetectDialog;
}

class CameraConfigDlg;

class CameraDetectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CameraDetectDialog(QWidget *parent = nullptr);
    ~CameraDetectDialog();
    void ClearnLoggerInfo();
    void SetInitialState();
private:
    void SetUpConnections();
    void DisableCameraSettingOption();
    void EnableCameraSettingOption(int nums);
    QString findServerByResourceName(const char* ResourceName,const QStringList& ServerNames);
private:
    int camCount{};
    CameraConfigs* m_CamerasConfig{nullptr};
    CameraConfigDlg* m_CamCfgDialog{nullptr};
private slots:
    void DetectCamerasSlot();
    void EmitCamerasConfigs();
    void SetCam1ConfigsSlot();
    void SetCam2ConfigsSlot();
    void SetCam3ConfigsSlot();
    void SetCam4ConfigsSlot();
    void SetCam5ConfigsSlot();
    void SetCam6ConfigsSlot();
    void SetCam7ConfigsSlot();
    void SetCam8ConfigsSlot();
    void SaveCurrentConfigSlot();
    void LoadCameraConfigsSlot();
private:
    Ui::CameraDetectDialog *ui;
    QStringList CameraServerNames;
    QVector<QString> CameraVenders;
signals:
    void sendToLogger(QString,QColor);
    void sendCameraConfigs(int,CameraConfigs*);
};

class CameraConfigDlg : public QDialog
{
    Q_OBJECT
public:
    CameraConfigDlg(QStringList& DetectedNames,QWidget* parent = nullptr);
    ~CameraConfigDlg();
    void updateComboBox();
private:
    void setDisplay();
private:
    QStringList& CameraNames;
public:
    QLabel* m_ServerName{nullptr};
    QLabel* m_configFile{nullptr};
    QLabel* FlatFieldFile{nullptr};

    QLineEdit* m_configPathEdit{nullptr};
    QLineEdit* m_FlatFieldEdit{nullptr};

    QPushButton* m_pickConfigFileBtn{nullptr};
    QPushButton* m_pickFlatPathBtn{nullptr};
    QPushButton* m_confirmBtn{nullptr};

    QComboBox* m_comboBox{nullptr};
    QGridLayout* mainLayout;
private slots:
    void pickConfigFileSlot();
    void pickFlatFileSlot();
};


#endif // CAMERADETECTDIALOG_H

















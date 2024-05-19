#ifndef CAMERABASIC_H
#define CAMERABASIC_H

#include <QObject>
#include <QDebug>
#include <SapClassBasic.h>
#include <SapManager.h>

//#define DEV_SHOWCAMTIMESTAMP_ENABLE
#define MAX_GIO_DEVICE	12

typedef void (*CALLBACKFUNC)(QString,void*);  //回调函数，用于指示当前相机图像采集完成了, 接受参数为相机的名称

struct ImageSize
{
  int height;
  int width;
  ImageSize(){height = 0;width=0;}
  ImageSize(int val1,int val2) {height = val1;width=val2;}
};


class CameraBasic : public QObject
{
    Q_OBJECT
public:
    explicit CameraBasic(QObject *parent = nullptr);
    ~CameraBasic();
    bool SetCamConnect(QString ServerName,QString CamVender,QString CamConfigFile,QString ffPath);
    void SetCamDisConnection();
    bool DisConnectionTransfer();
    bool ConnectTransfer();
    bool ClearnBuffer();
    bool SetCamReadyToSnap();   //创建Transfer
    bool StartSnap();
    bool StartGrab();   //用于调光
    bool IsExternalTriggerOn(); //为False则进行Snap,否则进行Grab
    bool StopGrab();    //关闭采集
    bool StopSnap();
    bool EnableFlatField();
    bool DisableFlatField();
    bool ISFlatFieldEnabled();
    ImageSize getImageSize();
    void SetHostCallBack(CALLBACKFUNC funPtr);  //只需要设置一次,由第一个创建的相机进行调用
    QString GetCameraVender() {return m_camVender;}
    void EmitCurrentImage();
    QString GetFFtPath() const {return m_FlatFieldPath;}
    void SetValidPath(QString Path,QString venderName);

private:
    void RegisterDeviceCallBacks();
    void UnRegisterDeviceCallBacks();
    bool IsValidFlatFieldFile(QString);
public:
    static void TransferCallBack(SapXferCallbackInfo* pInfo);       //负责采集图像
    static void SignalCallBack(SapAcqDeviceCallbackInfo* pInfo);    //指示相机事件
protected:
    SapLocation* loc{nullptr};
    SapFeature* CamFeature{nullptr};
    SapAcqDevice* device{nullptr};
    SapBufferWithTrash* buffer{nullptr};    //image Buffer
    SapTransfer* Xfer{nullptr};
    SapFlatField* Flatfield{nullptr};
private:
    static CALLBACKFUNC imageStateCallBack;
    bool m_step1Done{false};
    bool m_step2Done{false};
    QString m_camVender;
    QString m_FlatFieldPath;
    QString m_ServerName;
    QString m_ConfigFile;
public:
    template<class T>
    bool SetFeatureVal(const char* featureName,T Val)
    {
        if(device->SetFeatureValue(featureName,Val))
        {
            return true;
        }
         else
        {
            return false;
        }
    }
    template<class T>
    bool GetFeatureVal(const char* featureName,T& val)
    {
        if(device->GetFeatureValue(featureName,&val))
            return true;
        else
            return false;
    }
signals:

};
/*
 *imgPtr -> hostImgPtr
 *
 *
 *
 *
 */

#endif // CAMERABASIC_H

#include "camerabasic.h"

CALLBACKFUNC CameraBasic::imageStateCallBack = nullptr;

CameraBasic::CameraBasic(QObject *parent) : QObject(parent)
{

}

CameraBasic::~CameraBasic()
{
    SetCamDisConnection();
    qDebug()<<"Destroy "<<m_camVender<<" Instance...";
}

bool CameraBasic::SetCamConnect(QString ServerName,QString CamVender,QString CamConfigFile,QString ffPath)
{
    //创建Loc,Feature,Device,Gio.Tranfer稍后创建
    //m_FlatFieldPath
    m_ServerName = ServerName;
    m_FlatFieldPath = ffPath;
    m_camVender = CamVender;    //用于指示是哪一个相机,和centerWidget中的名称对应
    loc = new SapLocation{ServerName.toStdString().c_str(),0};
    CamFeature = new SapFeature{*loc};
    CamFeature->Create();
    m_ConfigFile = CamConfigFile;
    if(CamConfigFile != "None"){
        device = new SapAcqDevice{*loc,CamConfigFile.toStdString().c_str()};
    }
    else
        device = new SapAcqDevice{*loc};

    Flatfield = new SapFlatField{device};
    //CameraIOCallBack
    m_step1Done = device->Create() && Flatfield->Create();
    RegisterDeviceCallBacks();
    return m_step1Done;
}


bool CameraBasic::SetCamReadyToSnap()
{
    if(m_step1Done){
        buffer = new SapBufferWithTrash{3,device};
        Xfer = new SapAcqDeviceToBuf{device,buffer,TransferCallBack,this};
        m_step2Done = buffer->Create() && Xfer->Create();
    }
    return m_step2Done;
}

bool CameraBasic::ClearnBuffer()
{
    if(buffer != nullptr){
        buffer->ResetIndex();
        return buffer->Clear();
    }
    return false;
}

bool CameraBasic::DisConnectionTransfer()
{
    if(m_step2Done){
    if(Xfer->IsGrabbing()){
        Xfer->Freeze();
        if(!Xfer->Wait(1000))
            Xfer->Abort();
    }
        return ClearnBuffer() && Xfer->Disconnect();    //重置和清空buffer的缓冲区
    }
    return false;
}

bool CameraBasic::ConnectTransfer()
{
    if(!Xfer->IsConnected())
        return Xfer->Connect();
    return false;
}


bool CameraBasic::StartSnap()
{
    if(Xfer != nullptr && Xfer->IsConnected()){
        return Xfer->Snap();
    }
    return false;
}

bool CameraBasic::StopSnap()
{
    return Xfer->Abort();
    return false;
}

bool CameraBasic::IsExternalTriggerOn()
{
    if(device != nullptr){
        if(device->GetFeatureInfo("TriggerMode",CamFeature)){
            int enumIdx{};
            if(device->GetFeatureValue("TriggerMode",&enumIdx)){
                char enumString[MAX_PATH];
                CamFeature->GetEnumStringFromValue(enumIdx,enumString,MAX_PATH);
                if(strcmp(enumString,"On") == 0)
                    return true;
                else
                    return false;
            }
        }
    }
    return false;
}

bool CameraBasic::StartGrab()
{
    if(Xfer != nullptr && Xfer->IsConnected() && !Xfer->IsGrabbing()){
        return Xfer->Grab();
    }
    return false;
}

bool CameraBasic::StopGrab()
{
    if(Xfer->IsGrabbing()){
        Xfer->Freeze();
        if(!Xfer->Wait(500))
            return  Xfer->Abort();
    }
    return true;
}

void CameraBasic::SetValidPath(QString Path,QString name)
{
    if(name == "FFT")
        m_FlatFieldPath = Path;
    else if(name == "CCF")
        m_ConfigFile = Path;
}

void CameraBasic::RegisterDeviceCallBacks()
{

    device->RegisterCallback("AcquisitionStart",SignalCallBack,this);
    device->RegisterCallback("AcquisitionEnd",SignalCallBack,this);
    device->RegisterCallback("ImageLost",SignalCallBack,this);
    device->RegisterCallback("InvalidLineTrigger",SignalCallBack,this);
    device->RegisterCallback("eventsOverflow",SignalCallBack,this);
    device->RegisterCallback("LineTriggerOverlap",SignalCallBack,this);

}

void CameraBasic::UnRegisterDeviceCallBacks()
{
    device->UnregisterCallback("AcquisitionStart");
    device->UnregisterCallback("AcquisitionEnd");
    device->UnregisterCallback("ImageLost");
    device->UnregisterCallback("InvalidLineTrigger");
    device->UnregisterCallback("eventsOverflow");
    device->UnregisterCallback("LineTriggerOverlap");
}
void CameraBasic::SetHostCallBack(CALLBACKFUNC funPtr)
{
    imageStateCallBack = funPtr;
}


void CameraBasic::SetCamDisConnection()
{
    //销毁所有已经创建的实例
    if(m_step2Done){
        StopSnap();
        //Xfer->UnregisterCallback();
        DisConnectionTransfer();
        //Xfer->Disconnect();
        Xfer->Destroy();
        delete Xfer;
        Xfer = nullptr;
        buffer->Destroy();
        delete buffer;
        buffer = nullptr;
    }
    if(m_step1Done){
        UnRegisterDeviceCallBacks();
        device->Destroy();
        delete device;
        device = nullptr;
        CamFeature->Destroy();
        delete CamFeature;
        CamFeature = nullptr;
        delete loc;
        Flatfield->Destroy();
        delete Flatfield;
        Flatfield = nullptr;
    }
    m_step2Done = false;
    m_step1Done = false;
    //imageStateCallBack = nullptr;
}

void CameraBasic::EmitCurrentImage()
{
    void* imagePtr;
    buffer->GetAddress(buffer->GetIndex(),&imagePtr);
    if(imageStateCallBack != nullptr)
        imageStateCallBack(m_camVender,imagePtr);
}

ImageSize CameraBasic::getImageSize()
{
    ImageSize res{};
    if(buffer != nullptr){
        res.width = buffer->GetWidth();
        res.height = buffer->GetHeight();
    }

    return res;
}





void CameraBasic::SignalCallBack(SapAcqDeviceCallbackInfo* pInfo)
{
    //qDebug()<<"Enter Device CallBack...";
    CameraBasic* pClass = (CameraBasic*)(pInfo->GetContext());
    int eventIndex;
    char eventName[256];
    pInfo->GetEventIndex(&eventIndex);
    pInfo->GetAcqDevice()->GetEventNameByIndex(eventIndex, eventName, sizeof(eventName));
    qDebug()<<pClass->GetCameraVender()<<" Event Name: "<<eventName;
}


void CameraBasic::TransferCallBack(SapXferCallbackInfo *pInfo)  //一帧取完
{
    //采集一行发一行,能取到下一个source的图像,(一个source的资源不会自动覆盖)
    CameraBasic* pClass = (CameraBasic*)(pInfo->GetContext());
#ifdef DEV_SHOWCAMTIMESTAMP_ENABLE
    UINT64 timestmp{0};
    pInfo->GetAuxiliaryTimeStamp(&timestmp);
    qDebug()<<pClass->GetCameraVender()<<" timestmp : "<<timestmp;
#endif
    if(pInfo->IsTrash())
        qDebug()<<pClass->GetCameraVender()<<" buffer is Trash...";
    SapXferPair::EventType type =  pInfo->GetEventType();
    if(type == SapXferPair::EventEndOfFrame){   //唯一的事件类型
        pClass->EmitCurrentImage();    //centerWidget中的回调函数将被调用
        //qDebug()<<pClass->GetCameraVender()<<" frame over...";
        //将下一个index清零
    }
}


bool CameraBasic::IsValidFlatFieldFile(QString)
{
    return true;
}

bool CameraBasic::EnableFlatField()
{
    if(m_FlatFieldPath != "None" && IsValidFlatFieldFile(m_FlatFieldPath))
    if(Flatfield->Load(m_FlatFieldPath.toStdString().c_str())){
        return Flatfield->Enable();
    }
    return false;
}

bool CameraBasic::DisableFlatField()
{
    if(ISFlatFieldEnabled()){
        return Flatfield->Enable(FALSE);
    }
    return false;
}

bool CameraBasic::ISFlatFieldEnabled()
{
    return Flatfield->IsEnabled();
}
























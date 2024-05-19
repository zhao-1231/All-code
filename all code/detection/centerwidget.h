#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QTextEdit>
#include <QTableView>
#include <QImage>
#include <QMap>
#include <QTimer>
#include <chrono>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QProxyStyle>
#include <QDockWidget>
#include <QMutex>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <iostream>
#include "camerabasic.h"
#include "basicitem.h"
#include "inceptionresultmodel.h"
#include "cameradetectdialog.h"
#include "imageprocessthread.h"
#include "measurementsettingdialog.h"
#include "grascaletunerdia.h"
#include "signaltuner.h"
#pragma execution_character_set("utf-8")
typedef std::chrono::high_resolution_clock::time_point tp;


//#define DEV_TUNE_IMAGE_PROCESS

class CenterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CenterWidget(QWidget *parent = nullptr);
    ~CenterWidget();
    void DetectCamerasSlot();
    bool ConnectCamerasSlot();
    void DisConnectCamerasSlot();
    void StartSeqInception();  //开始边采集边检测
    void StopSeqInception();   //停止边采集边检测检测
    void StartInception();  //开始检测
    void StopInception();   //停止检测
    void StartGrab();   //开始连续采集
    void StopGrab();    //停止连续采集
    void AllocateCameraMemory();
    bool IsCamExternTriggerOn();
    void LoadExitCalibRes();    //加载已有尺寸标定参数
    void SWitchInceptionMode(QString mode);
    void lockMutexLocker();
private:
    void SaveTuneImage();
private:
    void InitialLizeImageState();
    void CreatGroupProcessingThread();
    void DestroyGroupProcessingThread();
    void RegisterCamManagerCallBack();
private:
    static void ImageReceiveFunction(QString,void*);
    static void ServerCallback(SapManCallbackInfo *pInfo);
    static void HandleMutexStaticFunction();      //负责处理线程的锁问题
    static CenterWidget* selfPtr;
public:
    static QVector<QString> CameraVenders;
    QMap<QString,bool> ImageState;
    QMap<QString,long> CamImageRecord;
    QMap<QString,cv::Mat*> ImageSources;
    void SetImageState(QString vender); //表示当前相机采图结束
    void SetImageSource(QString vender,void*); //表示当前相机采图结束
    void IncreaseCamImgNums(QString vender);
    void CheckGroupOneReady(QString vender);
    void CheckGroupTwoReady();
    //外加Check,所有图像应该在1ms内都采集结束
private:
    void SetOverAllUpDisPlay();
    void SetDockPriporty();
    void SetUpDisplayDock();
    void SetUpInceptionResDock();
    void SetUpsysInfoDock();
    void SetUpSubWindow();  //创建子窗口
    void FreeAllCameras();
    void StartGroupOneThread();
    void StartGroupTwoThread();
private:
    //main Form
    QHBoxLayout* mainLayout;
    QSplitter* mg_splitterMain{nullptr};
    QSplitter* mg_leftSplitter{nullptr};
    QDockWidget* mg_DisplayDock;
    QDockWidget* mg_inceptionResDock;
    QDockWidget* mg_leftBtmDockWidget;
    QDockWidget* mg_DefectsResDock;
private:
    QImage* m_ShowImage{nullptr};
private:
    //image DisPlay
    BasicItem* m_basicItem{nullptr};
    QGraphicsView* mg_imageViewer{nullptr};
    QGraphicsScene* mg_imageScene{nullptr};
private:
    //inception data display
    InceptionResultModel* m_dataModel{nullptr};
    QTableView* mg_inceptionDataViewer{nullptr};
private:
    //subDisplay Window
    CameraDetectDialog* mg_CameraDetecter{nullptr};
    GrascaleTunerDia* mg_grascalTunerDia;
    SignalTuner* mg_sigTunerDia;
private:
    //system outinfo
    QTextEdit* m_syslogger{nullptr};
private:
    //Handle Camera Connections
    int m_camCount{};
    int m_groupOneNums{};
    int m_groupTwoNums{};
    CameraConfigs* m_camCfgs{nullptr};
    QVector<CameraBasic*> LineScanCameras;  //管理相机的Vector
private:
    //相机采集
    bool IsMemoryAllocated{false};
    //QTimer* m_checkTimer{};//设定计时1ms,所有相机需要采集结束,否则报错
    bool m_ISInHandle{false};
private:
    //图像拼接参数和分区像素尺寸
    MeasureMentSettingDialog* m_measureStingDia;
private:
    ImageProcessThread* m_GroupOnePThread{nullptr};
    QMutex* group1_mutex{nullptr};
    bool m_IsMainThreadKeepLock{false};
private:
    //连续检测的参数,连续多少帧图片构成完整采图,进行尺寸检测以及缺陷汇总
    int m_separationTotalImage{28};  //默认为7  ->引用方式传入处理线程中
    bool m_SequentialInceptionConfiged{false};
    //bool m_mainImageCreated{false}; //连续采集的条件下用于存储总图像，指定帧数采集完成后进行显示和完整性检测(将各个小图像拷贝到该图像中)
    std::vector<cv::Mat*> m_seqImages;  //多个小图像,由处理线程依据拼接后的图像尺寸进行创建->引用方式传入线程中
    //大图像可以将原先的图像重复使用
    //cv::Mat* m_seqmainImage;
    bool m_IsOnSepInception{false};     //检测模式切换
    int m_SeqprocessedNums{};
private:
    //连续检测相关函数
    bool CheckSequenInceptionStting();  //用于检测各个相机采集的行数是否符合规范
    void ResetImageIndex();             //停止采集之后将小图像索引置为0,
    //void CreatSequntialMainImage();
    //void StartSequntialInception();
private slots:
    void LockSplitterPartition();
    void UnLockSplitterPartition();
    void AppendLoggerContent(QString contents,QColor color);
    void receiveCameraConfigsSlot(int,CameraConfigs*);
    void groupOneImageAcquiredSlot();
    void groupTwoImageAcquiredSlot();
    //void lockgroupOneMutexSlot();
    void ShowImageProcessingResultSlot();
    void SaveImageSlot();
    void MeasureMentSettingSlot();  //设置重叠和测量参数
    void ColorBalanceSlot();    //调节灰度平衡
    void SignalTriggerSlot();   //分频和倍频调节
signals:
    void CanConnectCameras();
    void SetSoftWareToinitialState();
};

#endif // CENTERWIDGET_H

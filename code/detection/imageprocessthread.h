#ifndef IMAGEPROCESSTHREAD_H
#define IMAGEPROCESSTHREAD_H

#include <QThread>
#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QVector>
#include <QMap>
#include <QMutex>
#include <QDateTime>
#include <QMap>
#include <atomic>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <algorithm>
#include <bitset>
#include "inceptionresultmodel.h"
#include "defectdetectionthread.h"

//#define DEV_MEASUREMENT_TEST
//#define DEV_UPLOAD_MEASURE_RESULT
//#define DEV_TUNE_IMAGE_PROCESS

struct SepmeasureCalibRes
{
    double h_val;
    double w_val;
    double theta;
    SepmeasureCalibRes(double h,double w,double t){h_val = h;w_val = w;theta = t;}
};

typedef void (*lockCallBack)();

class ImageProcessThread : public QThread
{
    Q_OBJECT
private:
    struct PotencialPt
    {
        int px;
        int py;
        void operator=(const PotencialPt& elem){px = elem.px;py = elem.py;}
        PotencialPt(){px=0;py=0;}
        PotencialPt(int tpx,int tpy){px = tpx;py = tpy;}
    };

    struct LineInfo
    {
        float slope;
        float interVal;
        LineInfo(){slope = 0;interVal=0;}
        LineInfo(float val1,float val2) {slope = val1;interVal = val2;}
    };

    struct MeasurePoint
    {
        double x;
        double y;
        double angle;
        bool status;
        MeasurePoint(){x = 0;y=0;angle = 0;status=false;}
        MeasurePoint(float v1,float v2,float v3,bool s){x= v1;y=v2;angle = v3;status=s;}
        void reset(){x = 0;y=0;angle = 0;status=false;}
    };

public:
    struct ExtractLineStatus
    {
        bool FirstLineS;
        bool SecondLineS;
        ExtractLineStatus() {FirstLineS = false;SecondLineS = false;}
        ExtractLineStatus(bool s1,bool s2) {FirstLineS = s1;SecondLineS = s2;}
    };

public:
    ImageProcessThread(QImage*& showImage,QMap<QString,cv::Mat*>& theElem,QMutex*& mutex,int& cam_nums,\
                       bool& ownLock,int& n_sep,std::vector<cv::Mat*>& sepImage,bool& Is_sepInception,int& SeqprocessedNums);
    ~ImageProcessThread();
    void run() override;
    void SetStichParameters(QVector<int> params);
    QVector<int> GetStichParameters(){return m_StichVlaidindex;}
    void StopTheThread(){canRun = false;}
    void unlockMutex(){qDebug()<<"proc thread detroy mutex";m_mutex->tryLock();m_mutex->unlock();}
    void SetDataModel(InceptionResultModel* thePtr);
    void UpLoadMeasureInfo();
    bool LoadExitCalibRes();    //加载分段标定参数
    void setMutexCallBack(lockCallBack mutexCallFunc);    //回调,用于阻塞线程
public:
    //边采集边检测相关的函数
    void SepIncepReConsuctions();
private:
    void FindPotencialEdgePoints(cv::Mat& Image,int k = -1);
    ExtractLineStatus FilterPoints(std::vector<cv::Point2f>& Line1,std::vector<cv::Point2f>& Line2,std::vector<PotencialPt>& result);
    void RefineLine(const std::vector<cv::Point2f>& originLine,std::vector<cv::Point2f>& dstLine);
    LineInfo GetLineInfo(const std::vector<cv::Point2f>& points,cv::Mat Image);
    cv::Point3f ExtratPrecisePt(cv::Point2f roughPt,cv::Mat& theImage,int oritation);
    int TryFilterPoints(std::vector<PotencialPt>& result);
    bool IsEdgeInImage(std::vector<cv::Point2f>& theElems);
    void extractCorners(cv::Mat& theImage);
    bool doubleCheck(int jumpedIndex,float previous_slope,std::vector<PotencialPt> result);
    bool isSingleGlassInImage(int thresHold,cv::Mat& Image);
    double CalphysicalLength(MeasurePoint pt1,MeasurePoint pt2);
    QVector<int> GetAllPotencialJumpedPoints(std::vector<PotencialPt> result);
    int getJumpedIndex(std::vector<PotencialPt>& result);
    bool ISlegalPotencialPt(const cv::Mat& Image,int x,int y);   //LBP CHECK
    void initialStdEdgeLBP();
    std::bitset<48> CalCulateLBPDecriptor(const cv::Mat& Image,int x,int y);
    void SaveTuneImage();
private:
    QImage*& m_showImage;
    QMap<QString,cv::Mat*>& m_images;
    QMutex*& m_mutex;
    lockCallBack m_lockFunc;
    int& m_camCount;
    bool& IsOwnLock;
    cv::Mat* m_StichedImage{nullptr};
private:
    //Stich Params 
    bool canRun{true};
    QVector<int> m_StichVlaidindex; //有效像素点的位置
    QVector<int> m_StichOffset; //暂时不用
private:
    void StichImageFunction();
    void ResetImage();
private:
    std::vector<PotencialPt> result1;   //left->right
    std::vector<PotencialPt> result2;   //right->left
    MeasurePoint topLeft{};
    MeasurePoint topRight{};
    MeasurePoint btmLeft{};
    MeasurePoint btmRight{};
    InceptionResultModel* dataModel{nullptr};
private:
    //尺寸参数
    double delta_x{0.11572};
    double delta_y{0.112675};
    double theta_val{1.68788e-06};
    int GlassId{1};
    cv::Mat processImage;
private:
    //分段标定参数设置
    float m_signalFac{1.};
    int m_toalSeps{};
    int m_PixelsPerSep{};
    int m_stichedImageCols{};
    std::vector<SepmeasureCalibRes> m_measureCalibRes;
    //标准边界LBP特征
    std::bitset<48> m_stdEdgeLBP[4];
private:
    //边采集边检测相关变量
    int& m_sepNumImages;
    std::vector<cv::Mat*>& m_sepImages;
    bool& m_IssepInception;
    int& m_SeqprocessedNums;   //不断递增用于指示当前小图像的次序当m_SeqprocessedNums == m_sepNumImages,开始显示和尺寸检测
    bool m_IsmainImageProcessed{true};


public:
    void LockDefectMutexLocker();   //内鬼

private:
    void InvokeInceptionDefectThread();
private:
    //控制缺陷检测线程
    QMutex m_defectDetecionLocker;  //互斥锁
    DefectDetectionThread* m_defectInceptionThread{nullptr};
    std::vector<cv::Mat> m_floatSepImages;
    bool m_keepDefectIncepLocker{false};
private:
    static ImageProcessThread* selfPtr; //用于静态回调函数中,访问非静态成员
    void initilizeDefectInceptionThread();  //初始化缺陷检测线程
private:
    static void MutexLockerCallback();  //回调函数
signals:
    void ShowImageReady();
    //void MutexUnlocked();//由回调函数进行控制了
    void ImageProcessDone();
};

#endif // IMAGEPROCESSTHREAD_H

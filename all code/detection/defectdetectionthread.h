#ifndef DEFECTDETECTIONTHREAD_H
#define DEFECTDETECTIONTHREAD_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QThread>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <vector>
#include <QDebug>
#include <QString>
#include "cudaHeader.cuh"

typedef void (*FuncPtr)();

class DefectDetectionThread : public QThread
{
    Q_OBJECT
public:
    DefectDetectionThread(QMutex& locker,std::vector<cv::Mat>& sep_images);
    ~DefectDetectionThread();
    void setMutexCallBack(FuncPtr funcPtr);
    void BreakWhile(){canRun=false;}
    void run() override;    //重写纯虚函数
    QString GetDeviceName();
    bool ISCurrentImageProcessed(){return m_currentImageProcessed;}
    void ResetImageIndex(){m_processedIdx = 0;}
private:
    void unlockMutex(); //让出互斥锁,指示主线程占有锁
    void copyImageToDeviceMemory(int idx);
    void setDeviceGridBlock();
public:

    //void setCurrentIndex(int idx){m_neededProcessingImage.push_back(idx);} //inline function
    void calralevantParams();
private:
    QMutex& m_locker;
    bool canRun{true};
    std::vector<cv::Mat>& m_sepImages;  //主线程向其中添加图片,释放同样由主线程控制
    //std::vector<int> m_neededProcessingImage;   //待处理的图像序号
    FuncPtr m_unlocker;
private:

    QString m_device_name;
private:


private:
    int m_pitch;    //二维内存对齐
    int m_imgWid;   //图像宽度
    int m_imgHei;   //图像高度
    int m_totalPixels;  //图像像素总数
    int m_hgradSize{8}; //cell(HOG)的尺寸
    int m_wgradSize{8}; //cell(HOG)的尺寸
    int m_wGrads;   //图像宽度可以划分的网格数量
    int m_hGrads;   //图像高度可以划分的网格数量
    int m_totalGrads;   //一张图像可以划分的网格数量
    int m_wBlocks;      //水平方向上block的数量
    int m_hBlocks;      //垂直方向上block的数量
    int m_totalBlocks;  //全部block的数量
    int m_foldSep;      //归约使用的粒度
private:

    float* m_deviceImageData;   //图像内存(设备中)
    float* m_devicegrads;   //存储图像梯度的内存(设备中)
    float* m_deviceangles;  //存储图像梯度方向的内存(设备中)
    float* m_devicehisto;   //存储HOG直方图信息的内存(设备中)
    float* m_devicehog;     //存储HOG算子的内存(设备中)
    bool m_deviceMemoryAllcated{false};
private:

    int m_bins; //hog特征使用的bin设置
    int m_sepAngle; //(180 / m_bins)
    int m_gridSize; //Hog特征一个grid的尺寸
private:
    dim3 m_gradientGrid;    //cuda计算梯度时分配的grid大小
    dim3 m_gradientBlock;   //cuda计算梯度时分配的block大小

    dim3 m_hogCellGrid;     //cuda计算hog描述子分配的grid大小
    int m_cellPerColumn;    //图像每一行包含的cell(HOG)个数
    int m_cellPerRaw{};     //图像每一列包含的cell(HOG)个数
    dim3 m_hogCellBlcok;    //cuda计算hog描述子分配的block大小

    int m_blockPerCol;      //图像每一行包含的block(HOG)个数
    int m_blockPerRaw;      //图像每一列包含的block(HOG)个数
    dim3 m_hogDescriptorGrid;   //统计hog描述子时分配的grid数量
    dim3 m_hogDescriptorBlock;  //统计hog描述子时分配的block数量
private:
    int m_processedIdx{};   //当前处理到哪张图像了
    bool m_currentImageProcessed{false};
private:

    float* host_boxes;  //存储检测的结果,整个中间计算结果全部在设备中
};

#endif // DEFECTDETECTIONTHREAD_H

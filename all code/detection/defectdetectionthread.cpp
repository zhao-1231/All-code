#include "defectdetectionthread.h"

#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))
static void HandleError(cudaError_t err, const char* file, int line)
{
    // CUDA error handeling from the "CUDA by example" book
    if (err != cudaSuccess)
    {
        printf("%s in %s at line %d\n", cudaGetErrorString(err), file, line);
        exit(EXIT_FAILURE);
    }
}


DefectDetectionThread::DefectDetectionThread(QMutex& locker,std::vector<cv::Mat>& sep_images) : m_locker(locker),m_sepImages(sep_images)
{


}


DefectDetectionThread::~DefectDetectionThread()
{
    cudaDeviceReset();
}

void DefectDetectionThread::unlockMutex()
{
    m_locker.unlock();  //让出锁
    m_unlocker();   //指示主线程占有锁
}

void DefectDetectionThread::setMutexCallBack(FuncPtr funcPtr)
{
    m_unlocker = funcPtr;   //funcPtr为回调函数(静态)
}


void DefectDetectionThread::run()
{
    //死循环
    while(canRun){
        m_locker.lock();    //阻塞住,主线程解锁,继续向下处理
        if(!canRun){
            m_locker.unlock();
            break;
        }
        //无待处理的图像
        if(m_sepImages.empty())//无待处理的图像,直接让出锁,处理完成
        {
            unlockMutex();  //让出锁,同时主线程拥有锁
        }

        //拿到当前待处理帧图像,并分配设备内存,将图像数据拷贝到设备内存中

        //设置计算使用的grid和block
        //setDeviceGridBlock();
        //calralevantParams();
        copyImageToDeviceMemory(m_processedIdx);
        //提取基础特征

        roughGradientExcuter(m_deviceImageData,m_devicegrads,m_deviceangles,m_pitch,m_imgWid,m_imgHei,m_foldSep,m_gradientGrid,m_gradientBlock);
        //构建hog描述子
        HANDLE_ERROR(cudaGetLastError());
        HANDLE_ERROR(cudaDeviceSynchronize());


        //计算直方图
        HOGGridHistogram(m_devicegrads,m_deviceangles,m_devicehisto,m_sepAngle,m_bins,m_imgWid,m_imgHei,\
                         m_gridSize,m_cellPerColumn,m_hogCellGrid,m_hogCellBlcok);
        HANDLE_ERROR(cudaGetLastError());
        HANDLE_ERROR(cudaDeviceSynchronize());

        //统计HOG特征
        HOGDecriptor(m_devicehisto,m_devicehog,m_cellPerColumn,m_cellPerRaw,m_blockPerCol,m_bins,m_hogDescriptorGrid,m_hogDescriptorBlock);
        HANDLE_ERROR(cudaGetLastError());
        HANDLE_ERROR(cudaDeviceSynchronize());

        //当前图像处理完成,将其移除
        //m_neededProcessingImage.erase(m_neededProcessingImage.begin());
        m_processedIdx += 1;
        qDebug()<<"processed image idx: "<<m_processedIdx;
        if(m_sepImages.size() == m_processedIdx) //目前存入的图像全部处理完成，线程阻塞,否则继续取图进行处理
        {
            unlockMutex();  //让出锁,同时主线程拥有锁
             //图像vector中增加图片,主线程都会释放锁,让该线程处理,直到已经处理的图片数和vector中存储的图片数相等,表示图片处理完成
            m_currentImageProcessed = true;
        }else{
            m_locker.unlock(); //自己解锁,进行下一次循环
            m_currentImageProcessed = false;
        }
           //全部图像还未处理完成
        //实施分类器,存入检测结果

        //将当前帧检测结果返回调用的线程

        //让出锁

        //resetDevice
    }
    qDebug()<<"Break While Loop...";
}

void DefectDetectionThread::setDeviceGridBlock()
{
    m_foldSep = 4;
    m_gradientBlock.x = 32;
    m_gradientBlock.y = 16;
    qDebug()<<"in DefectDetectionThread m_imgWid: "<<m_imgWid;
    qDebug()<<"in DefectDetectionThread m_imgHei: "<<m_imgHei;
    m_gradientGrid.x = (m_imgWid + (m_foldSep*m_gradientBlock.x) - 1) / (m_gradientBlock.x * m_foldSep);
    m_gradientGrid.y = (m_imgHei + (m_foldSep*m_gradientBlock.y) - 1) / (m_gradientBlock.y * m_foldSep);
    qDebug()<<"m_gradientGrid.x: "<<m_gradientGrid.x;
    qDebug()<<"m_gradientGrid.y: "<<m_gradientGrid.y;

    m_bins = 9;
    m_sepAngle = 180 / 9;
    m_gridSize = 8;
    m_hogCellBlcok.x = 32;
    m_hogCellBlcok.y = 16;
    //计算图像宽度方向上含有多少的cell
    m_cellPerColumn = m_imgWid / m_gridSize;
    if(m_imgWid % m_gridSize)
        m_cellPerColumn += 1;
    m_cellPerRaw = m_imgWid / m_gridSize;
    if(m_cellPerRaw % m_gridSize)
        m_cellPerRaw += 1;
    m_hogCellGrid.x = (m_cellPerColumn + m_hogCellBlcok.x - 1) / m_hogCellBlcok.x;
    m_hogCellGrid.y = (m_cellPerRaw + m_hogCellBlcok.y - 1) / m_hogCellBlcok.y;
    m_blockPerCol = m_cellPerColumn - 1;
    m_blockPerRaw = m_cellPerRaw - 1;

    m_hogDescriptorBlock.x = 32;
    m_hogDescriptorBlock.y = 16;

    m_hogDescriptorGrid.x = (m_blockPerCol + m_hogDescriptorBlock.x - 1) / m_hogDescriptorBlock.x;
    m_hogDescriptorGrid.y = (m_blockPerRaw + m_hogDescriptorBlock.y - 1) / m_hogDescriptorBlock.y;
}

QString DefectDetectionThread::GetDeviceName()
{
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop,0);
    m_device_name = prop.name;
    return m_device_name;
}


void DefectDetectionThread::calralevantParams()
{
    //计算图像可以划分的网格数量
    m_wGrads = m_imgWid / m_wgradSize;
    if(m_imgWid % m_wgradSize)
        m_wGrads += 1;

    m_hGrads = m_imgHei / m_hgradSize;
    if(m_imgHei % m_hgradSize)
        m_imgHei += 1;
    m_totalGrads = m_wGrads * m_hGrads;

    m_wBlocks = m_wGrads;
    m_hBlocks = m_hGrads;
    m_totalBlocks = m_wBlocks * m_hBlocks;

}

void DefectDetectionThread::copyImageToDeviceMemory(int idx)
{
    cv::Mat& tmpImage = m_sepImages.at(idx);
    //在设备上分配图像内存
    m_imgWid = tmpImage.cols;
    m_imgHei = tmpImage.rows;
    qDebug()<<"image width: "<<m_imgWid;
    qDebug()<<"image height: "<<m_imgHei;
    m_totalPixels = m_imgWid * m_imgHei;
    setDeviceGridBlock();
    calralevantParams();
    if(!m_deviceMemoryAllcated){
        cudaDeviceReset();
    //为图像分配内存,内存对齐
        HANDLE_ERROR(cudaMallocPitch((float**)(&m_deviceImageData),(size_t*)&m_pitch,size_t(m_imgWid*sizeof(float)),(size_t)m_imgHei));
        //将图像数据拷贝到设备内存当中
        //在设备上分配存储角度的内存
        HANDLE_ERROR(cudaMalloc((float**)(&m_devicegrads),(m_totalPixels*sizeof(float))));
        HANDLE_ERROR(cudaMalloc((float**)(&m_deviceangles),(m_totalPixels* sizeof(float))));
        qDebug()<<"total allocated bytes: "<<m_totalGrads * m_bins * sizeof(float);
        HANDLE_ERROR(cudaMalloc((float**)(&m_devicehisto),(m_totalGrads * m_bins * sizeof(float))));
        HANDLE_ERROR(cudaMalloc((float**)(&m_devicehog),(m_totalBlocks * m_bins * sizeof(float))));
        HANDLE_ERROR(cudaMemcpy2D(m_deviceImageData,m_pitch,tmpImage.data,m_imgWid*sizeof(float),m_imgWid*sizeof(float),m_imgHei,cudaMemcpyHostToDevice));
    }
    //将图像数据拷贝到相应的内存中
    //HANDLE_ERROR(cudaMemcpy2D(m_deviceImageData,m_pitch,tmpImage.data,m_imgWid*sizeof(float),m_imgWid*sizeof(float),m_imgHei,cudaMemcpyHostToDevice));
}






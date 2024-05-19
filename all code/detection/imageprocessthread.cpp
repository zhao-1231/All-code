#include "imageprocessthread.h"

ImageProcessThread* ImageProcessThread::selfPtr = nullptr;

ImageProcessThread::ImageProcessThread(QImage*& showImage,QMap<QString,cv::Mat*>& theElem,QMutex*& mutex,int& cam_nums,bool& ownLock,\
                                       int& n_sep,std::vector<cv::Mat*>& sepImage,bool& Is_sepInception,int& SeqprocessedNums) : \
    m_showImage(showImage),m_images(theElem),m_mutex(mutex),m_camCount(cam_nums),IsOwnLock(ownLock),\
    m_sepNumImages(n_sep),m_sepImages(sepImage),m_IssepInception(Is_sepInception),m_SeqprocessedNums(SeqprocessedNums)
{

    /*modi*/
    QFile file{R"(./OverLap.json)"};
    if(file.open(QIODevice::ReadOnly)){
    QJsonParseError pErr;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),&pErr);
    if(!doc.isNull()){
        //emit sendToLogger(pErr.errorString(),Qt::red);
        //now loading contents
        QJsonArray array = doc.array();
        for(int i{};i<array.count();++i){
            m_StichVlaidindex.push_back(array.at(i)["OverLap"].toInt());
            qDebug()<<"in index "<<i<<" val: "<<array.at(i)["OverLap"].toInt();
        }
        file.close();
    }
    }else{
        qDebug()<<"open OverLapSetting.json error...";
        m_StichVlaidindex.push_back(0);  //61
        m_StichVlaidindex.push_back(0);
        m_StichVlaidindex.push_back(0);
    }
    Q_ASSERT(m_StichVlaidindex.size() == 3);

    m_stichedImageCols += theElem["CameraA"]->cols;
    for(int i{};i<m_StichVlaidindex.size();++i)
        m_stichedImageCols += (theElem["CameraA"]->cols - m_StichVlaidindex.at(i));
    qDebug()<<"stiched image cols: "<<m_stichedImageCols;
    qDebug()<<"image rows : "<<theElem["CameraA"]->rows;
    m_StichedImage = new cv::Mat(theElem["CameraA"]->rows,m_stichedImageCols,CV_8UC1);
    qDebug()<<"m_StichedImage Created...";
    m_showImage = new QImage{m_StichedImage->data,m_StichedImage->cols,m_StichedImage->rows,m_StichedImage->cols,QImage::Format_Grayscale8};
    qDebug()<<"m_showImage Created...";
    selfPtr = this;
    initilizeDefectInceptionThread();   //创建缺陷检测线程
    qDebug()<<"defect detection Thread Create Success...";
}

ImageProcessThread::~ImageProcessThread()
{
    if(m_StichedImage != nullptr){
        delete m_StichedImage;
        m_StichedImage = nullptr;
    }
    if(m_showImage != nullptr){
        delete m_showImage;
        m_showImage = nullptr;
    }
    qDebug()<<"destroy ImageProcessThread....";
    /*****************释放缺陷检测相关资源*************************/
    //释放锁
    m_defectInceptionThread->BreakWhile();
    InvokeInceptionDefectThread();   //释放锁,while再次循环时会跳出
    m_defectInceptionThread->quit();
    m_defectInceptionThread->wait(200);
    delete m_defectInceptionThread;
    m_defectInceptionThread = nullptr;
    qDebug()<<"defect detection Thread Release Success...";
}

void ImageProcessThread::ResetImage()
{
    if(m_showImage != nullptr){
        delete m_showImage;
        m_showImage = nullptr;
    }
    if(m_StichedImage != nullptr){
        delete m_StichedImage;
        m_StichedImage = nullptr;
    }
}

void ImageProcessThread::StichImageFunction()
{
    QDateTime curTime = QDateTime::currentDateTime();
    qDebug()<<"start time : "<<curTime;
    cv::Rect Roi_A{0,0,m_images["CameraA"]->cols,m_images["CameraA"]->rows};
    cv::Rect Roi_B{m_StichVlaidindex.at(0),0,m_images["CameraA"]->cols-m_StichVlaidindex.at(0),m_images["CameraA"]->rows};
    cv::Rect Roi_C{m_StichVlaidindex.at(1),0,m_images["CameraA"]->cols-m_StichVlaidindex.at(1),m_images["CameraA"]->rows};
    cv::Rect Roi_D{m_StichVlaidindex.at(2),0,m_images["CameraA"]->cols-m_StichVlaidindex.at(2),m_images["CameraA"]->rows};

    cv::Rect Roi_Bdst{m_images["CameraA"]->cols,0,m_images["CameraA"]->cols-m_StichVlaidindex.at(0),m_images["CameraA"]->rows};
    int nums = m_images["CameraA"]->cols*2 - m_StichVlaidindex.at(0);
    cv::Rect Roi_Cdst{nums,0,m_images["CameraA"]->cols-m_StichVlaidindex.at(1),m_images["CameraA"]->rows};
    nums += (m_images["CameraA"]->cols - m_StichVlaidindex.at(1));
    cv::Rect Roi_Ddst{nums,0,m_images["CameraA"]->cols-m_StichVlaidindex.at(2),m_images["CameraA"]->rows};

    if(!m_IssepInception){
         m_images["CameraA"]->copyTo((*m_StichedImage)(Roi_A));
        (*m_images["CameraB"])(Roi_B).copyTo((*m_StichedImage)(Roi_Bdst));
        (*m_images["CameraC"])(Roi_C).copyTo((*m_StichedImage)(Roi_Cdst));
        (*m_images["CameraD"])(Roi_D).copyTo((*m_StichedImage)(Roi_Ddst));
    }else{
        //将当前采集到的图像拷贝到各个小图像中
        qDebug()<<"current sepration index: "<<m_SeqprocessedNums;

        if(m_SeqprocessedNums <= m_sepNumImages){
            /*
            for(int i{};i<m_images["CameraA"]->rows;++i){
                uchar* startDstPtr = m_sepImages.at(m_SeqprocessedNums-1)->data + i * m_sepImages.at(m_SeqprocessedNums-1)->cols;
                uchar* startAPtr = m_images["CameraA"]->data + i * m_images["CameraA"]->cols;
                uchar* startBPtr = m_images["CameraB"]->data + i * m_images["CameraB"]->cols + m_StichVlaidindex.at(0);
                uchar* startCPtr = m_images["CameraC"]->data + i * m_images["CameraC"]->cols + m_StichVlaidindex.at(1);
                uchar* startDPtr = m_images["CameraD"]->data + i * m_images["CameraD"]->cols + m_StichVlaidindex.at(2);
                std::move(startAPtr,startAPtr+m_images["CameraA"]->cols,startDstPtr);
                std::move(startBPtr,startBPtr+m_images["CameraB"]->cols - m_StichVlaidindex.at(0),startDstPtr + m_images["CameraA"]->cols);
                int nums = m_images["CameraA"]->cols + m_images["CameraB"]->cols - m_StichVlaidindex.at(0);
                std::move(startCPtr,startCPtr+m_images["CameraC"]->cols - m_StichVlaidindex.at(1),startDstPtr + nums);
                nums += m_images["CameraC"]->cols - m_StichVlaidindex.at(1);
                std::move(startDPtr,startDPtr+m_images["CameraD"]->cols - m_StichVlaidindex.at(2),startDstPtr + nums);
            }
            */
            m_images["CameraA"]->copyTo((*(m_sepImages.at(m_SeqprocessedNums-1)))(Roi_A));
           (*m_images["CameraB"])(Roi_B).copyTo((*(m_sepImages.at(m_SeqprocessedNums-1)))(Roi_Bdst));
           (*m_images["CameraC"])(Roi_C).copyTo((*(m_sepImages.at(m_SeqprocessedNums-1)))(Roi_Cdst));
           (*m_images["CameraD"])(Roi_D).copyTo((*(m_sepImages.at(m_SeqprocessedNums-1)))(Roi_Ddst));
            //小图像拼接完成,传入到缺陷检测线程中
            cv::Mat tmpImage;
            m_sepImages.at(m_sepNumImages-1)->convertTo(tmpImage,CV_32F);
            m_floatSepImages.push_back(std::move(tmpImage));    //检测线程收到图像
            //m_floatSepImages[m_sepNumImages-1] = tmpImage;
            //msleep(50);
            //设置当前图像缩影
            //m_defectInceptionThread->setCurrentIndex(m_sepNumImages-1);
            //开始检测缺陷
            if(m_keepDefectIncepLocker) //1、缺陷检测线程阻塞中. 2、缺陷检测线程提前检测完所有的图片将锁归还
                InvokeInceptionDefectThread();  //互斥锁被释放,缺陷检测线程开始处理(如果当前线程拥有锁)

        //将拼接完成的小图像拷贝到大图像中(设置条件,当大图像尺寸检测完成才可以进行拷贝)
        if(m_IsmainImageProcessed){
            //cv::Rect Roi_rect{0,m_images["CameraA"]->rows*(m_SeqprocessedNums-1),m_stichedImageCols,m_images["CameraA"]->rows};
            int total_pixels = m_sepImages.at(m_SeqprocessedNums-1)->rows * m_sepImages.at(m_SeqprocessedNums-1)->cols;
            std::move(m_sepImages.at(m_SeqprocessedNums-1)->data,m_sepImages.at(m_SeqprocessedNums-1)->data+total_pixels,\
                      m_StichedImage->data+total_pixels*(m_SeqprocessedNums-1));
            //m_sepImages.at(m_SeqprocessedNums-1)->copyTo((*m_StichedImage)(Roi_rect));
        }
        }
    }
    curTime = QDateTime::currentDateTime();
    qDebug()<<"end time : "<<curTime;
}

void ImageProcessThread::SetStichParameters(QVector<int> params)
{
    if(params[0] > 0 && params[1] > 0 && params[2] > 0){
        m_StichVlaidindex = params;
        ResetImage();
        m_stichedImageCols = 0;
        m_stichedImageCols += m_images["CameraA"]->cols;
        for(int i{};i<m_StichVlaidindex.size();++i)
            m_stichedImageCols += (m_images["CameraA"]->cols - m_StichVlaidindex.at(i));
        m_StichedImage = new cv::Mat(m_images["CameraA"]->rows,m_stichedImageCols,CV_8UC1);
        m_showImage = new QImage{m_StichedImage->data,m_StichedImage->cols,m_StichedImage->rows,m_StichedImage->cols,QImage::Format_Grayscale8};
    }
}

void ImageProcessThread::SetDataModel(InceptionResultModel* thePtr)
{
    dataModel = thePtr;
}


void ImageProcessThread::SaveTuneImage()
{
    QString str = R"(C:\Users\Y_hua\Desktop\TuneImage)";
    if(!str.isEmpty()){
        QDateTime curTime = QDateTime::currentDateTime();
        QString ImageASaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamA.jpg";
        QString ImageBSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamB.jpg";
        QString ImageCSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamC.jpg";
        QString ImageDSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "CamD.jpg";
        QString ImageSSaveName = str + "//" + curTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + "StichedImage.jpg";

        if(m_StichedImage != nullptr){
            //QPixmap srcPixmap = QPixmap::fromImage(*m_ShowImage);
            //srcPixmap.save(ImageSSaveName);
            cv::imwrite(ImageDSaveName.toStdString(),*m_StichedImage);
        }
    }
}

void ImageProcessThread::setMutexCallBack(lockCallBack mutexCallFunc)
{
    m_lockFunc = mutexCallFunc; //设置锁回调阻塞
}

void ImageProcessThread::run()
{
    while(canRun){
        m_mutex->lock();
        IsOwnLock = false;  //主线程未拥有锁

        if(!canRun){
            m_mutex->unlock();
            break;
        }

        StichImageFunction();
        if(m_SeqprocessedNums == m_sepNumImages){   //全部图像接受完成,开始尺寸检测
            m_SeqprocessedNums = 0; //主线程负责调整接收到的小图像个数,处理线程处理完成将其置为0
            //缺陷检测线程相关,检查缺陷检测线程是否将当前帧全部处理完成

            m_IsmainImageProcessed = false;
            emit ShowImageReady();
            m_IsmainImageProcessed = true;

#ifdef DEV_TUNE_IMAGE_PROCESS
        SaveTuneImage();
#endif

#ifdef DEV_MEASUREMENT_TEST

        //processImage  =~(*m_StichedImage);
        cv::threshold(*m_StichedImage,processImage,20,255,cv::THRESH_BINARY);
        //cv::medianBlur(processImage,processImage,3);  //比较耗时,如果图像干净，可以去除该步骤
        if(isSingleGlassInImage(500,processImage)){
            FindPotencialEdgePoints(processImage,-1);
            extractCorners(processImage);
            UpLoadMeasureInfo();
            emit ImageProcessDone();
        }
#endif
        qDebug()<<"size measurement over...";
        bool s = m_defectInceptionThread->ISCurrentImageProcessed();
        while(!s){s = m_defectInceptionThread->ISCurrentImageProcessed();}
        qDebug()<<"defect detection over...";
        //将图像处理线程的图像索引归零
        }

        m_mutex->unlock(); //解开线程锁,由主线程控制
        m_lockFunc();   //当前帧处理完成
    }
}

bool ImageProcessThread::LoadExitCalibRes()
{
    QString path = QFileDialog::getOpenFileName(nullptr,"Load Calib File",QString(),("json file(*.json)"));
    QFile file{path};
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    QJsonParseError pErr;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),&pErr);
    if(doc.isNull()){
        file.close();
        return false;
    }
    QJsonArray array = doc.array();
    qDebug()<<"array cout: "<<array.count();
    m_PixelsPerSep = array.at(0)["total Seprations"].toInt();
    m_PixelsPerSep = array.at(0)["pexels per sep"].toInt();
    m_measureCalibRes.clear();
    for(int i{1};i<array.count();++i){
        double widthVal = array.at(i)["widthVal"].toDouble();
        double heightVal = array.at(i)["heightVal"].toDouble();
        double thetaVal = array.at(i)["thetaVal"].toDouble();
        m_measureCalibRes.push_back({heightVal,widthVal,thetaVal});
    }
    file.close();
    return true;
}


void ImageProcessThread::initialStdEdgeLBP()
{
    int cur_idx{};
    for(int r{1};r<=3;++r){
        for(int i{};i<8 * r;++i){
            if((2*r < i) && (i < 6*r + 1)){
                m_stdEdgeLBP[0][cur_idx] = 0;
                m_stdEdgeLBP[2][cur_idx] = 1;
            }
            if(i < int(r * 4 - 1)){
                m_stdEdgeLBP[1][cur_idx] = 0;
                m_stdEdgeLBP[3][cur_idx] = 1;
            }
            ++cur_idx;
        }
    }
}

std::bitset<48> ImageProcessThread::CalCulateLBPDecriptor(const cv::Mat& Image,int x,int y)
{
    /*
     * x,y 坐标
     * retrun : LBP原始描述子(8+16+24)
    */
    int cur_bitIdx{};
    int center_val = int(Image.at<uchar>(y,x));
    std::bitset<48> res;
    int radius[3]{1,2,3};
    for(int i{};i<3;++i){
        int P = radius[i] * 8;
        for(int p{};p<P;++p){
            float nei_x = x + radius[i] * cos(CV_2PI * p / P);
            float nei_y = y - radius[i] * sin(CV_2PI * p / P);
            //插值(最近邻插值)
            int p_x = int(round(nei_x));
            int p_y = int(round(nei_y));
            if(p_x < 0) p_x = 0;
            if(p_x >= Image.cols) p_x = Image.cols - 1;
            if(p_y < 0) p_y = 0;
            if(p_y >= Image.rows) p_y = Image.rows - 1;
            int nei_val = int(Image.at<uchar>(p_y,p_x));
            res[cur_bitIdx] = nei_val >= center_val ? 1 : 0;
            ++cur_bitIdx;
        }
    }
    return res;
}

bool ImageProcessThread::ISlegalPotencialPt(const cv::Mat& Image,int x,int y)
{
    int thres = 18;
    //计算潜在点与LBP边缘特征之间的汉明距离
    std::bitset<48> ptLBP = CalCulateLBPDecriptor(Image,x,y);
    for(int i{};i<4;++i){
        int xor_res = (ptLBP ^ m_stdEdgeLBP[i]).count();
        if(xor_res < thres) //汉明距离小于18
            return true;
    }
    return false;
}

void ImageProcessThread::FindPotencialEdgePoints(cv::Mat& Image,int k)
{
    int initial_bias = 50;
    int increase_ses = 50;
    result1.clear();
    result2.clear();
    for(int bias = initial_bias;bias < ((Image.rows-1) - k * (Image.cols-1)) - increase_ses;bias += increase_ses){
        int end_posX = bias / std::abs(k);
        end_posX = std::min(end_posX,Image.cols-1);
        PotencialPt edge1,edge2;
        bool s1{false},s2{false};
        for(int x{1};x< Image.cols;++x){    //forward
            int y = k * x + bias;
            if(y < Image.rows && y >= 0){
            if(Image.at<uchar>(y,x) == 255){
                //more check


                int valid_nums{0};
                for(int c{std::min(x+1,end_posX)};c<=std::min(x+10,end_posX);++c){
                    int tmpy = k*c+bias;
                    if(tmpy<Image.rows && Image.at<uchar>(tmpy,c) == 255)
                        ++valid_nums;
                }
                if(valid_nums > 3){
                    edge1.px = x;
                    edge1.py = y;
                    s1 = true;
                    break;
                }


                /*
                if(ISlegalPotencialPt(Image,x,y)){
                    edge1.px = x;
                    edge1.py = y;
                    s1 = true;
                }
                */
           }
            }
        }

        for(int x{Image.cols-1};x > 1;--x){     //backward
            int y = k * x + bias;
            if(y < Image.rows && y >= 0){
                //qDebug()<<".......................";
            if(Image.at<uchar>(y,x) == 255){
                //more check

                int valid_nums{0};
                for(int c{std::max(x-1,0)};c >= std::max(x-10,0);--c){
                    int tmpy = k*c+bias;
                    if(tmpy < Image.rows && Image.at<uchar>(k*c+bias,c) == 255)
                        ++valid_nums;
                }
                if(valid_nums > 3){
                    edge2.px = x;
                    edge2.py = y;
                    s2 = true;
                    break;
                }




               /*
                if(ISlegalPotencialPt(Image,x,y)){
                    edge2.px = x;
                    edge2.py = y;
                    s2 = true;
                }
                */

            }
        }
        }
        if(s1)
           result1.push_back(edge1);
        if(s2)
            result2.push_back(edge2);
    }
    //qDebug()<<"points size: "<<result1.size() + result2.size();
    qDebug()<<"result1 size: "<<result1.size();
    qDebug()<<"result2 size: "<<result2.size();
    /*
    qDebug()<<"result1: ";
    for(const auto& elem : result1){
        qDebug()<<"x: "<<elem.px<<" y:"<<elem.py;
    }

    qDebug()<<"result2: ";
    for(const auto& elem : result1){
        qDebug()<<"x: "<<elem.px<<" y:"<<elem.py;
    }
    */

}

bool ImageProcessThread::isSingleGlassInImage(int thresHold,cv::Mat& Image)
{
    int glass_nums{};
    bool checked{false};
    for(int j{};j<Image.rows;j+=100){
        int valid_nums{};
        for(int i{};i<Image.cols;++i){
            if(Image.at<uchar>(j,i) == 255){
                ++valid_nums;

            }
        }
        if(valid_nums > thresHold && !checked){
            checked = true;
            ++glass_nums;
        }
        if(valid_nums < int(thresHold / 5) && checked){
            checked = false;
        }
    }
    qDebug()<<"glass nums: "<<glass_nums;
    if(glass_nums == 1)
        return true;
    return false;
}


bool ImageProcessThread::doubleCheck(int jumpedIndex,float previous_slope,std::vector<PotencialPt> result)
{
    //往后Check 5 个,5个全部是跳变点则返回True
    int InValid_nums{};
    float current_slope{};
    float angleThres = 20.;
    for(int index{jumpedIndex+1};index< result.size() -1;++index){    //jumpedIndex为跳变点
        if(result.at(index).px == result.at(index+1).px)
            current_slope = INFINITY;
        else
            current_slope = float(result.at(index).py - result.at(index+1).py) / float(result.at(index).px - result.at(index+1).px);
        if(previous_slope == INFINITY || current_slope == INFINITY){
            if(previous_slope == INFINITY && current_slope == INFINITY){
                //ok,next loop
                ++InValid_nums;
            }
            else if(previous_slope == INFINITY){
                float angleDiff = 90. - std::abs(std::atan(current_slope)) / CV_PI * 180.;
                //qDebug()<<"angleDiff:"<<angleDiff;
                if(angleDiff < angleThres){ //still ok,
                    ++InValid_nums;
                }
            }else{
                float angleDiff = 90. - std::abs(std::atan(previous_slope)) / CV_PI * 180.;
                //qDebug()<<"angleDiff:"<<angleDiff;
                if(angleDiff < angleThres){ //still ok,
                    ++InValid_nums;
                }
            }
        }
        else{
            float angleDiff = std::atan(std::abs((current_slope - previous_slope) / (1. + current_slope * previous_slope))) / CV_PI * 180.;
            //qDebug()<<"angleDiff:"<<angleDiff;
            if(angleDiff < angleThres){
                ++InValid_nums;
                ;
            }
        }
    }
    if(InValid_nums == 0){
        //qDebug()<<"checked index : "<<jumpedIndex<<" result: "<<"true";
        return true;
    }
    else{
        //qDebug()<<"checked index : "<<jumpedIndex<<" result: "<<"false";
        return false;
    }

}



int ImageProcessThread::TryFilterPoints(std::vector<PotencialPt>& result)
{
    //跳变之后无跳变,Line2中有值
    float angleThres = 20.; //20度
    bool jumped{false};
    int jumpedIndex{};
    float previous_slope{};
    if(result.at(1).px != result.at(0).px)
        previous_slope = float(result.at(1).py - result.at(0).py) / float(result.at(1).px - result.at(0).px);
    else
        previous_slope = INFINITY;
    float current_slope{};
    for(int i{1};i<result.size()-1;++i){
        if(jumped)
            break;
        if(result.at(i).px == result.at(i+1).px)
            current_slope = INFINITY;
        else
            current_slope = float(result.at(i+1).py - result.at(i).py) / float(result.at(i+1).px - result.at(i).px);
        if(previous_slope == INFINITY || current_slope == INFINITY){
            if(previous_slope == INFINITY && current_slope == INFINITY){
                //ok,next loop
                previous_slope = current_slope;
                jumpedIndex = i + 1;
            }
            else if(previous_slope == INFINITY){
                float angleDiff = 90. - std::abs(std::atan(current_slope)) / CV_PI * 180.;
                if(angleDiff < angleThres){ //still ok,
                    previous_slope = current_slope;
                    jumpedIndex = i + 1;
                }else{  //jumped,开始检索Line2
                    jumpedIndex = i + 1;    // jumpedIndex为第二条边开始的索引
                    jumped = doubleCheck(jumpedIndex,previous_slope,result);
                    //jumped = true;
                }
            }else{
                float angleDiff = 90. - std::abs(std::atan(previous_slope)) / CV_PI * 180.;
                if(angleDiff < angleThres){ //still ok,
                    jumpedIndex = i + 1;
                    previous_slope = current_slope;
                }else{
                    jumpedIndex = i + 1;
                    jumped = doubleCheck(jumpedIndex,previous_slope,result);
                    //jumped = true;
                }
            }

        }
        else{
            float angleDiff = std::atan(std::abs((current_slope - previous_slope) / (1. + current_slope * previous_slope))) / CV_PI * 180.;
            if(angleDiff < angleThres){
                previous_slope = current_slope;
                jumpedIndex = i + 1;
            }else{
                jumpedIndex = i + 1;
                //jumped = true;
                jumped = doubleCheck(jumpedIndex,previous_slope,result);
            }
        }

    }

    return jumpedIndex;
}


QVector<int> ImageProcessThread::GetAllPotencialJumpedPoints(std::vector<PotencialPt> result)   //还需要修改噪点移除部分
{
    QVector<int> points_result;
    float angleThres = 20.; //20度
    int jumpedIndex{};
    float previous_slope{};
    if(result.at(1).px != result.at(0).px)
        previous_slope = float(result.at(1).py - result.at(0).py) / float(result.at(1).px - result.at(0).px);
    else
        previous_slope = INFINITY;
    float current_slope{};
    for(int i{1};i<result.size()-1;++i){
        if(result.at(i).px == result.at(i+1).px)
            current_slope = INFINITY;
        else
            current_slope = float(result.at(i+1).py - result.at(i).py) / float(result.at(i+1).px - result.at(i).px);
        if(previous_slope == INFINITY || current_slope == INFINITY){
            if(previous_slope == INFINITY && current_slope == INFINITY){
                //ok,next loop
                previous_slope = current_slope;
                jumpedIndex = i + 1;
            }
            else if(previous_slope == INFINITY){
                float angleDiff = 90. - std::abs(std::atan(current_slope)) / CV_PI * 180.;
                if(angleDiff < angleThres){ //still ok,
                    previous_slope = current_slope;
                    jumpedIndex = i + 1;
                }else{  //jumped,开始检索Line2
                    jumpedIndex = i + 1;    // jumpedIndex为第二条边开始的索引
                    if((i+2 < result.size())){
                        if(result.at(i+1).px == result.at(i+2).px)
                            previous_slope = INFINITY;
                        else
                            previous_slope = float(result.at(i+2).py - result.at(i+1).py) / float(result.at(i+2).px - result.at(i+1).px);
                    }else{
                        previous_slope = current_slope;
                    }
                    points_result.push_back(jumpedIndex);
                }
            }else{
                float angleDiff = 90. - std::abs(std::atan(previous_slope)) / CV_PI * 180.;
                if(angleDiff < angleThres){ //still ok,
                    jumpedIndex = i + 1;
                    previous_slope = current_slope;
                }else{
                    jumpedIndex = i + 1;
                    if((i+2 < result.size())){
                        if(result.at(i+1).px == result.at(i+2).px)
                            previous_slope = INFINITY;
                        else
                            previous_slope = float(result.at(i+2).py - result.at(i+1).py) / float(result.at(i+2).px - result.at(i+1).px);
                    }else{
                        previous_slope = current_slope;
                    }
                    points_result.push_back(jumpedIndex);
                }
            }
        }
        else{
            float angleDiff = std::atan(std::abs((current_slope - previous_slope) / (1. + current_slope * previous_slope))) / CV_PI * 180.;
            if(angleDiff < angleThres){
                previous_slope = current_slope;
                jumpedIndex = i + 1;
            }else{
                jumpedIndex = i + 1;
                if((i+2 < result.size())){
                    if(result.at(i+1).px == result.at(i+2).px)
                        previous_slope = INFINITY;
                    else
                        previous_slope = float(result.at(i+2).py - result.at(i+1).py) / float(result.at(i+2).px - result.at(i+1).px);
                }else{
                    previous_slope = current_slope;
                }
                points_result.push_back(jumpedIndex);
            }
        }

    }

    return points_result;
}


int ImageProcessThread::getJumpedIndex(std::vector<PotencialPt>& result)
{
    QVector<int> jumpedAndNoisePt = GetAllPotencialJumpedPoints(result);
    int total_volume = result.size();
    int maxIter = total_volume / 2;
    while(jumpedAndNoisePt.size() != 1 && maxIter > 0)
    {
        for(int i{jumpedAndNoisePt.size()-1};i>=0;--i){
            result.erase(result.begin() + jumpedAndNoisePt.at(i));
        }
        jumpedAndNoisePt = GetAllPotencialJumpedPoints(result);
        --maxIter;
    }
    if(maxIter != 0)
    {
        qDebug()<<"found jumed point: "<<jumpedAndNoisePt.at(0);
        qDebug()<<"total droped points: "<<int(total_volume / 2) - maxIter;
        qDebug()<<"remained result size:"<<result.size();
        return jumpedAndNoisePt.at(0);
    }else{
        qDebug()<<"too noise points,find jump point FAIL";
        return -1;
    }
}

ImageProcessThread::ExtractLineStatus ImageProcessThread::FilterPoints(std::vector<cv::Point2f>& Line1,std::vector<cv::Point2f>& Line2,std::vector<PotencialPt>& result)
{
    int jumpedIndex = getJumpedIndex(result);
    if(jumpedIndex > 2){
    for(int i{};i<result.size();++i){
        if(i < jumpedIndex){
            Line1.push_back({float(result.at(i).px),float(result.at(i).py)});
        }else{
            Line2.push_back({float(result.at(i).px),float(result.at(i).py)});
        }
    }
    }

    ExtractLineStatus status;
    if(Line1.size() > 2)
        status.FirstLineS = true;
    if(Line2.size() > 2)
        status.SecondLineS = true;

    return status;
}


bool ImageProcessThread::IsEdgeInImage(std::vector<cv::Point2f>& theElems)
{
    float Xoritation{},Yoritation{};
    float dataSize = theElems.size();
    for(const auto& elem : theElems){
        Xoritation += elem.x / dataSize;
        Yoritation += elem.y / dataSize;
    }
    bool s{true};
    if(std::abs(Xoritation) < 50 || std::abs(Xoritation - m_StichedImage->cols) < 50)   //贴边了
        s = false;
    if(std::abs(Yoritation) < 50 || std::abs(Yoritation - m_StichedImage->rows) < 50)   //贴边了
        s = false;

    return s;
}

void ImageProcessThread::RefineLine(const std::vector<cv::Point2f>& originLine,std::vector<cv::Point2f>& dstLine)
{
    //O(n^3)严格控制一条边上的点数
    float slope{};
    float interVal{};
    std::vector<float> errRates;
    for(int i{};i<originLine.size();++i){
        double errRate{};
        double calTimes{};
        for(int j{};j<originLine.size();++j){
            if(i != j){
                if(originLine.at(i).x == originLine.at(j).x){
                    slope = INFINITY;
                    interVal = originLine.at(i).x;
                }else{
                    slope = (originLine.at(i).y - originLine.at(j).y) / (originLine.at(i).x - originLine.at(j).x);
                    interVal = originLine.at(i).y - slope * originLine.at(i).x;
                }
                for(int k{};k<originLine.size();++k){
                    ++calTimes;
                    if(k != i && k != j){
                        if(slope == INFINITY)
                            errRate += std::abs(interVal - originLine.at(k).x);
                        else
                            errRate += std::abs(slope*originLine.at(k).x - originLine.at(k).y + interVal) / sqrt(1. + slope*slope);
                    }
                }


            }
        }
        errRates.push_back(errRate);
    }

    //剔除离群值
    double max_val = *std::max_element(errRates.cbegin(),errRates.cbegin());
    double thres_val = (std::accumulate(errRates.cbegin(),errRates.cend(),0.) - max_val) / (errRates.size()-1);
    for(int i{};i<errRates.size();++i){
        if(errRates.at(i) < thres_val)
            dstLine.push_back(originLine.at(i));
    }
}

ImageProcessThread::LineInfo ImageProcessThread::GetLineInfo(const std::vector<cv::Point2f>& points,cv::Mat Image)
{
    cv::Vec4f line_para;
    cv::Point point0;
    cv::Point point1, point2;
    if(points.size() > 2){
        cv::fitLine(points, line_para, cv::DIST_L2, 0, 1e-2, 1e-2);
        point0.x = line_para[2];
        point0.y = line_para[3];
        float k = line_para[1] / line_para[0];
        float interVal = line_para[3] - k * line_para[2];

        point1.x = 0;
        point2.x = Image.cols;
        point1.y = k * point1.x + interVal;
        point2.y = k * point2.x + interVal;
        cv::line(Image, point1, point2, cv::Scalar{255}, 1, cv::LINE_AA, 0);
        return LineInfo{k,interVal};
    }else{
        return LineInfo{0,0};
    }
}



void ImageProcessThread::extractCorners(cv::Mat& theImage)
{
    if(result1.size() == 0 || result2.size() ==0){
        qDebug()<<"Fatal Error no edges info";
        return;
    }

    //specifier result1
    std::vector<cv::Point2f> LineAD;
    std::vector<cv::Point2f> LineCD;
    std::vector<cv::Point2f> LineAB;
    std::vector<cv::Point2f> LineBC;
    LineInfo lAD,lCD,lAB,lBC;
    bool ADValid{false},CDValid{false},ABValid{false},BCValid{false};

    ExtractLineStatus s1 = FilterPoints(LineAD,LineCD,result1);
    //qDebug()<<"-------------------------------------------------------------";
    ExtractLineStatus s2 = FilterPoints(LineAB,LineBC,result2);
    if(s1.FirstLineS && IsEdgeInImage(LineAD)){
        std::vector<cv::Point2f> LineADrefined;
        RefineLine(LineAD,LineADrefined);
        lAD = GetLineInfo(LineADrefined,*m_StichedImage);
        ADValid = true;
        qDebug()<<"line AD Valid...";
        qDebug()<<"AD Slope: "<<lAD.slope;
        qDebug()<<"AD InterVal: "<<lAD.interVal;
    }
    if(s1.SecondLineS && IsEdgeInImage(LineCD)){
        std::vector<cv::Point2f> LineCDrefined;
        RefineLine(LineCD,LineCDrefined);
        lCD = GetLineInfo(LineCDrefined,*m_StichedImage);
        CDValid = true;
        qDebug()<<"line CD Valid...";
        /*
        for(const auto& elem : LineCD)
            qDebug()<<"X: "<<elem.x<<" y: "<<elem.y;
            */
        qDebug()<<"CD Slope: "<<lCD.slope;
        qDebug()<<"CD InterVal: "<<lCD.interVal;
    }
    if(s2.FirstLineS && IsEdgeInImage(LineAB)){
        std::vector<cv::Point2f> LineABrefined;
        RefineLine(LineAB,LineABrefined);
        lAB = GetLineInfo(LineABrefined,*m_StichedImage);
        ABValid = true;
        qDebug()<<"line AB Valid...";
        qDebug()<<"AB Slope: "<<lAB.slope;
        qDebug()<<"AB InterVal: "<<lAB.interVal;
    }
    if(s2.SecondLineS && IsEdgeInImage(LineBC)){
        std::vector<cv::Point2f> LineBCrefined;
        RefineLine(LineBC,LineBCrefined);
        lBC = GetLineInfo(LineBCrefined,*m_StichedImage);
        BCValid = true;
        qDebug()<<"line BC Valid...";
        qDebug()<<"BC Slope: "<<lBC.slope;
        qDebug()<<"BC InterVal: "<<lBC.interVal;
    }

    //top left
    if(ABValid && ADValid){
        float pt1_x = (lAB.interVal - lAD.interVal) / (lAD.slope - lAB.slope);
        float pt1_y = lAB.slope * pt1_x + lAB.interVal;

        topLeft.x = pt1_x;
        topLeft.y = pt1_y;
        qDebug()<<"top left x: "<<pt1_x<<" top left y : "<<pt1_y;
        cv::Point3f res = ExtratPrecisePt({pt1_x,pt1_y},theImage,1);
        qDebug()<<"precise x: "<<res.x<<" precise y: "<<res.y<<" angle: "<<res.z;
        topLeft.x = res.x;
        topLeft.y = res.y;
        topLeft.angle = res.z;
    }

    //top right
    if(ABValid && BCValid){
        float pt2_x = (lAB.interVal - lBC.interVal) / (lBC.slope - lAB.slope);
        float pt2_y = lAB.slope * pt2_x + lAB.interVal;

        topRight.x = pt2_x;
        topRight.y = pt2_y;
        qDebug()<<"top right x: "<<pt2_x<<" top right y : "<<pt2_y;
        cv::Point3f res = ExtratPrecisePt({pt2_x,pt2_y},theImage,2);
        qDebug()<<"precise x: "<<res.x<<" precise y: "<<res.y<<" angle: "<<res.z;
        topRight.x = res.x;
        topRight.y = res.y;
        topRight.angle = res.z;
    }

    //bottom right
    if(CDValid && BCValid){
        float pt3_x = (lBC.interVal - lCD.interVal) / (lCD.slope - lBC.slope);
        float pt3_y = lCD.slope * pt3_x + lCD.interVal;

        btmRight.x = pt3_x;
        btmRight.y = pt3_y;
        qDebug()<<"bottom right x: "<<pt3_x<<" bottom right y : "<<pt3_y;
        cv::Point3f res = ExtratPrecisePt({pt3_x,pt3_y},theImage,3);
        qDebug()<<"precise x: "<<res.x<<" precise y: "<<res.y<<" angle: "<<res.z;
        btmRight.x = res.x;
        btmRight.y = res.y;
        btmRight.angle = res.z;
    }

    //bottom left
    if(ADValid && CDValid){
        float pt4_x = (lCD.interVal - lAD.interVal) / (lAD.slope - lCD.slope);
        float pt4_y = lCD.slope * pt4_x + lCD.interVal;

        btmLeft.x = pt4_x;
        btmLeft.y = pt4_y;
        qDebug()<<"bottom left x: "<<pt4_x<<" bottom left y : "<<pt4_y;
        cv::Point3f res = ExtratPrecisePt({pt4_x,pt4_y},theImage,4);
        qDebug()<<"precise x: "<<res.x<<" precise y: "<<res.y<<" angle: "<<res.z;
        btmLeft.x = res.x;
        btmLeft.y = res.y;
        btmLeft.angle = res.z;
    }
}

double ImageProcessThread::CalphysicalLength(MeasurePoint m__pt1,MeasurePoint m__pt2)
{

    cv::Point2f m_pt1{float(m__pt1.x),float(m__pt1.y)};
    cv::Point2f m_pt2{float(m__pt2.x),float(m__pt2.y)};
    cv::Point2f pt1,pt2;
    if(m_pt1.x > m_pt2.x){
       pt1 = m_pt2;
       pt2 = m_pt1;
    }else{
        pt1 = m_pt1;
        pt2 = m_pt2;
    }

    float slope = (pt2.y - pt1.y) / (pt2.x - pt1.x);
    float intercept = pt1.y - slope * pt1.x;
    int startSep = pt1.x / m_PixelsPerSep;
    int endSep = pt2.x / m_PixelsPerSep;
    if(startSep == endSep){ //在同一分区中,直接计算
        float a = std::abs(pt1.x - pt2.x);
        float b = std::abs(pt1.y - pt2.y) * m_signalFac;
        SepmeasureCalibRes val = m_measureCalibRes.at(startSep);
        float len = powf(a*val.w_val,2.) + powf(b*val.h_val,2.) - 2*a*b*val.h_val*val.w_val*val.theta;
        qDebug()<<"phyLength: "<<sqrt(len);
        return sqrt(len);
    }else{
    //cv::Point2f startPt = pt1;
    float physicalLen{};
    //计算中间部分
    for(int i{startSep + 1};i<endSep;++i){
        SepmeasureCalibRes val = m_measureCalibRes.at(i);
        if(val.h_val == -1){
            qDebug()<<"encount invalid calib info";
            break;
        }
        float a = m_PixelsPerSep;
        //float b = std::abs((slope * i * m_PixelsPerSep + intercept) - (slope * (i+1) * m_PixelsPerSep + intercept));
        float b = std::abs(slope * m_PixelsPerSep) * m_signalFac;
        float len = powf(a*val.w_val,2.) + powf(b*val.h_val,2.) - 2*a*b*val.h_val*val.w_val*val.theta;
        physicalLen += sqrt(len);
    }
    //计算两端
    SepmeasureCalibRes val = m_measureCalibRes.at(startSep);
    if(val.h_val == -1){
        qDebug()<<"encount invalid calib info";
    }
    float a = std::abs(pt1.x - (startSep + 1) * m_PixelsPerSep);
    float b = std::abs(pt1.y - (slope*((startSep+1)*m_PixelsPerSep) + intercept)) * m_signalFac;
    float len = powf(a*val.w_val,2.) + powf(b*val.h_val,2.) - 2*a*b*val.h_val*val.w_val*val.theta;
    physicalLen += sqrt(len);

    val = m_measureCalibRes.at(endSep);
    if(val.h_val == -1){
        qDebug()<<"encount invalid calib info";
    }
    a = std::abs(pt2.x - endSep * m_PixelsPerSep);
    b = std::abs(pt2.y - (slope*(endSep*m_PixelsPerSep) + intercept)) * m_signalFac;
    len = powf(a*val.w_val,2.) + powf(b*val.h_val,2.) - 2*a*b*val.h_val*val.w_val*val.theta;
    physicalLen += sqrt(len);

    qDebug()<<"phyLength: "<<physicalLen;
    return physicalLen;
    }
}


void ImageProcessThread::UpLoadMeasureInfo()
{

#ifdef DEV_UPLOAD_MEASURE_RESULT
    QMap<QString,float> data;
    //float xDiff{},yDiff{};
    data["Name"] = GlassId;
    data["Angle(A)"] = topLeft.angle;
    data["Angle(B)"] = topRight.angle;
    data["Angle(C)"] = btmRight.angle;
    data["Angle(D)"] = btmLeft.angle;
    if(topLeft.x * topRight.x != 0 && topLeft.y * topRight.y != 0){
        //xDiff = abs(topLeft.x - topRight.x);
        //yDiff = abs(topLeft.y - topRight.y);
        data["Width(AB)"] = CalphysicalLength(topLeft,topRight);
    }
    else
        data["Width(AB)"] = -1.;
    if(btmRight.x * topRight.x != 0 && btmRight.y * topRight.y != 0){
        //xDiff = abs(btmRight.x - topRight.x);
        //yDiff = abs(btmRight.y - topRight.y);
        data["Height(BC)"] = CalphysicalLength(btmRight,topRight);
    }
    else
        data["Height(BC)"] = -1.;
    if(btmRight.x * btmLeft.x != 0 && btmRight.y * btmLeft.y != 0){
        //xDiff = abs(btmRight.x - btmLeft.x);
        //yDiff = abs(btmRight.y - btmLeft.y);
        data["Width(CD)"] = CalphysicalLength(btmRight,btmLeft);
    }
    else
        data["Width(CD)"] = -1.;
    if(topLeft.x * btmLeft.x != 0 && topLeft.y * btmLeft.y != 0){
        //xDiff = abs(topLeft.x - btmLeft.x);
        //yDiff = abs(topLeft.y - btmLeft.y);
        data["Height(AD)"] = CalphysicalLength(topLeft,btmLeft);
    }
    else
        data["Height(AD)"] = -1.;
    if(topLeft.x * btmRight.x != 0 && topLeft.y * btmRight.y != 0){
        //xDiff = abs(topLeft.x - btmRight.x);
        //yDiff = abs(topLeft.y - btmRight.y);
        data["Diag(AC)"] = CalphysicalLength(topLeft,btmRight);
    }
    else
        data["Diag(AC)"] = -1.;
    if(topRight.x * btmLeft.x != 0 && topRight.y * btmLeft.y != 0){
        //xDiff = abs(topRight.x - btmLeft.x);
        //yDiff = abs(topRight.y - btmLeft.y);
        data["Diag(BD)"] = CalphysicalLength(topRight,btmLeft);
    }
    else
        data["Diag(BD)"] = -1.;
    ++GlassId;
    if(dataModel != nullptr)
        dataModel->UpdataDataContainer(data);
#endif

}

cv::Point3f ImageProcessThread::ExtratPrecisePt(cv::Point2f roughPt,cv::Mat& theImage,int oritation)
{
    switch (oritation) {
    case 1: //Top Left
    {
        //edges 1 horizental
        if((roughPt.x >= theImage.cols || roughPt.x < 0) || (roughPt.y >= theImage.rows || roughPt.y < 0))
        {
            return cv::Point3f{0,0,-1};
        }
        std::vector<cv::Point2f> hLine;
        for(int x{int(MIN((roughPt.x + 200),theImage.cols-1))};x > int(MIN((roughPt.x + 50),theImage.cols-1));--x)
            for(int y{int(MAX(roughPt.y - 200,0))};y<theImage.rows;++y){
                if(theImage.at<uchar>(y,x) == 255){
                    //qDebug()<<"x: "<<x<<" y:"<<y;

                    int valid_nums = 0;
                    for(int c{MIN(y+1,theImage.rows-1)};c<MIN(y+11,theImage.rows-1);++c){
                        if(theImage.at<uchar>(c,x) == 255)
                            ++valid_nums;
                    }
                    if(valid_nums > 3){
                        hLine.push_back({float(x),float(y)});
                        break;
                    }

                }
            }
        //horizental done...

        //edges1 vertical
        std::vector<cv::Point2f> vLine;
        for(int y{int(MIN((roughPt.y + 200),theImage.rows-1))};y > int(int(MIN((roughPt.y + 50),theImage.rows-1)));--y)
            for(int x{int(MAX(roughPt.x - 200,0))};x<theImage.cols;++x){
                if(theImage.at<uchar>(y,x) == 255){
                    //qDebug()<<"x: "<<x<<" y:"<<y;

                    int valid_nums = 0;
                    for(int c{MIN(x+1,theImage.cols-1)};c<MIN(x+11,theImage.cols-1);++c){
                        if(theImage.at<uchar>(y,c) == 255)
                            ++valid_nums;
                    }
                    if(valid_nums > 3){
                        vLine.push_back({float(x),float(y)});
                        break;
                    }

                }

            }

        std::vector<cv::Point2f> hLinerefined;
        std::vector<cv::Point2f> vLinerefined;
        RefineLine(hLine,hLinerefined);
        RefineLine(vLine,vLinerefined);
        if(hLinerefined.empty() || vLinerefined.empty()){
            return cv::Point3f{-100,-100,-1};
        }
        LineInfo hLineInfo = GetLineInfo(hLinerefined,*m_StichedImage);
        LineInfo vLineInfo = GetLineInfo(vLinerefined,*m_StichedImage);
        float x = (vLineInfo.interVal - hLineInfo.interVal) / (hLineInfo.slope -vLineInfo.slope);
        float y = hLineInfo.slope * x + hLineInfo.interVal;
        float angle = atan(std::abs((vLineInfo.slope - hLineInfo.slope) / (1. + vLineInfo.slope * hLineInfo.slope))) / CV_PI * 180.;
        return cv::Point3f{x,y,angle};

        return cv::Point3f{0,0,-1};
        break;
    }
    case 2:
    {
        if((roughPt.x >= theImage.cols || roughPt.x < 0) || (roughPt.y >= theImage.rows || roughPt.y < 0))
        {
            return cv::Point3f{0,0,-1};
        }
        std::vector<cv::Point2f> hLine;
        for(int x{int(MAX(roughPt.x - 200,0))};x < int(MAX(roughPt.x - 50,0));++x)
            for(int y{int(MAX(roughPt.y - 200,0))};y<theImage.rows;++y){
                //qDebug()<<"x: "<<x<<" y:"<<y;
                if(theImage.at<uchar>(y,x) == 255){

                    int valid_nums = 0;
                    for(int c{MIN(y+1,theImage.rows-1)};c<MIN(y+11,theImage.rows-1);++c){
                        if(theImage.at<uchar>(c,x) == 255)
                            ++valid_nums;
                    }
                    if(valid_nums > 3){
                        hLine.push_back({float(x),float(y)});
                        break;
                    }

                }

            }

        //horizental done...

        //edges1 vertical

        std::vector<cv::Point2f> vLine;
        for(int y{int(MIN(roughPt.y + 200,theImage.rows-1))};y > int(MIN(roughPt.y + 50,theImage.rows-1));--y)
            for(int x{int(MIN(roughPt.x + 200,theImage.cols-1))};x>=0;--x){
                if(theImage.at<uchar>(y,x) == 255){
                    int valid_nums = 0;
                    for(int c{MAX(x-1,0)};c>MAX(x-11,0);--c){
                        if(theImage.at<uchar>(y,c) == 255)
                            ++valid_nums;
                    }
                    if(valid_nums > 3){
                        vLine.push_back({float(x),float(y)});
                        break;
                    }
                }
            }


        std::vector<cv::Point2f> hLinerefined;
        std::vector<cv::Point2f> vLinerefined;
        RefineLine(hLine,hLinerefined);
        RefineLine(vLine,vLinerefined);
        if(hLinerefined.empty() || vLinerefined.empty()){
            return cv::Point3f{0,0,-1};
        }

        LineInfo hLineInfo = GetLineInfo(hLinerefined,*m_StichedImage);
        LineInfo vLineInfo = GetLineInfo(vLinerefined,*m_StichedImage);

        float x = (vLineInfo.interVal - hLineInfo.interVal) / (hLineInfo.slope -vLineInfo.slope);
        float y = hLineInfo.slope * x + hLineInfo.interVal;
        float angle = atan(std::abs((vLineInfo.slope - hLineInfo.slope) / (1. + vLineInfo.slope * hLineInfo.slope))) / CV_PI * 180.;
        return cv::Point3f{x,y,angle};
        return cv::Point3f{0,0,-1};
        break;
    }
    case 4:
    {
        if((roughPt.x >= theImage.cols || roughPt.x < 0) || (roughPt.y >= theImage.rows || roughPt.y < 0))
        {
            return cv::Point3f{0,0,-1};
        }
        std::vector<cv::Point2f> hLine;
        for(int x{int(MIN(roughPt.x + 200,theImage.cols-1))};x > int(MIN(roughPt.x + 50,theImage.cols-1));--x)
            for(int y{int(MIN(roughPt.y + 200,theImage.rows-1))};y>0;--y){
                if(theImage.at<uchar>(y,x) == 255){
                    int valid_nums = 0;
                    for(int c{MAX(y-1,0)};c>MAX(y-11,0);--c){
                        if(theImage.at<uchar>(c,x) == 255)
                            ++valid_nums;
                    }
                    if(valid_nums > 3){
                        hLine.push_back({float(x),float(y)});
                        break;
                    }
                }
            }
        //horizental done...

        //edges1 vertical
        std::vector<cv::Point2f> vLine;
        for(int y{int(MAX(roughPt.y - 200,0))};y < int(MAX(roughPt.y - 50,0));++y)
            for(int x{int(MAX(roughPt.x - 200,0))};x<theImage.cols;++x){
                if(theImage.at<uchar>(y,x) == 255){
                    int valid_nums = 0;
                    for(int c{MIN(x+1,theImage.cols-1)};c<MIN(x+11,theImage.cols-1);++c){
                        if(theImage.at<uchar>(y,c) == 255)
                            ++valid_nums;
                    }
                    if(valid_nums > 3){
                        vLine.push_back({float(x),float(y)});
                        break;
                    }
                }
            }

        std::vector<cv::Point2f> hLinerefined;
        std::vector<cv::Point2f> vLinerefined;
        RefineLine(hLine,hLinerefined);
        RefineLine(vLine,vLinerefined);
        if(hLinerefined.empty() || vLinerefined.empty()){
            return cv::Point3f{0,0,-1};
        }

        LineInfo hLineInfo = GetLineInfo(hLinerefined,*m_StichedImage);
        LineInfo vLineInfo = GetLineInfo(vLinerefined,*m_StichedImage);

        float x = (vLineInfo.interVal - hLineInfo.interVal) / (hLineInfo.slope -vLineInfo.slope);
        float y = hLineInfo.slope * x + hLineInfo.interVal;
        float angle = atan(std::abs((vLineInfo.slope - hLineInfo.slope) / (1. + vLineInfo.slope * hLineInfo.slope))) / CV_PI * 180.;

        return cv::Point3f{x,y,angle};
        return cv::Point3f{0,0,-1};
        break;
    }
    case 3:
    {
        if((roughPt.x >= theImage.cols || roughPt.x < 0) || (roughPt.y >= theImage.rows || roughPt.y < 0))
        {
           return cv::Point3f{0,0,-1};
        }
        std::vector<cv::Point2f> hLine;
        for(int x{int(MAX(roughPt.x - 200,0))};x < int(MAX(roughPt.x - 50,0));++x)
            for(int y{int(MIN(roughPt.y + 200,theImage.rows-1))};y>0;--y){
                if(theImage.at<uchar>(y,x) == 255){
                    int valid_nums = 0;
                    for(int c{MAX(y-1,0)};c>MAX(y-11,0);--c){
                        if(theImage.at<uchar>(c,x) == 255)
                            ++valid_nums;
                    }
                    if(valid_nums > 3){
                        hLine.push_back({float(x),float(y)});
                        break;
                    }
                }
            }
        //horizental done...

        //edges1 vertical
        std::vector<cv::Point2f> vLine;
        for(int y{int(MAX(roughPt.y - 200,0))};y < int(MAX(roughPt.y - 50,0));++y)
            for(int x{int(MIN(roughPt.x + 200,theImage.cols-1))};x>0;--x){
                if(theImage.at<uchar>(y,x) == 255){
                    int valid_nums = 0;
                    for(int c{MAX(x-1,0)};c>MAX(x-11,0);--c){
                        if(theImage.at<uchar>(y,c) == 255)
                            ++valid_nums;
                    }
                    if(valid_nums > 3){
                        vLine.push_back({float(x),float(y)});
                        break;
                    }
                }
            }

        std::vector<cv::Point2f> hLinerefined;
        std::vector<cv::Point2f> vLinerefined;
        RefineLine(hLine,hLinerefined);
        RefineLine(vLine,vLinerefined);
        if(hLinerefined.empty() || vLinerefined.empty()){
            return cv::Point3f{0,0,-1};
        }

        LineInfo hLineInfo = GetLineInfo(hLinerefined,*m_StichedImage);
        LineInfo vLineInfo = GetLineInfo(vLinerefined,*m_StichedImage);

        float x = (vLineInfo.interVal - hLineInfo.interVal) / (hLineInfo.slope -vLineInfo.slope);
        float y = hLineInfo.slope * x + hLineInfo.interVal;
        float angle = atan(std::abs((vLineInfo.slope - hLineInfo.slope) / (1. + vLineInfo.slope * hLineInfo.slope))) / CV_PI * 180.;
        return cv::Point3f{x,y,angle};
        return cv::Point3f{0,0,-1};
        break;
    }
    default:
        return cv::Point3f{0,0,-1};
        break;
    }

    return cv::Point3f{0,0,0};
}

/**************************边检测边采集图像相关******************************************/
void ImageProcessThread::SepIncepReConsuctions()
{
    //首先创建基本的小图像
    for(auto& elem : m_sepImages)
        delete elem;
    m_sepImages.clear();
    for(int i{};i<m_sepNumImages;++i){
        cv::Mat* cacheImage = new cv::Mat(m_images["CameraA"]->rows,m_stichedImageCols,CV_8UC1);
        m_sepImages.push_back(cacheImage);
    }
    //利用m_StichedImage作为大图像
    if(m_StichedImage != nullptr){
        delete m_StichedImage;
        m_StichedImage = nullptr;
    }
    //每个拼接完成的小图像,复制到大图像中
    m_StichedImage = new cv::Mat(m_images["CameraA"]->rows * m_sepNumImages,m_stichedImageCols,CV_8UC1);
    qDebug()<<"m_StichedImage size: "<<m_StichedImage->cols<<" "<<m_StichedImage->rows;
    //显示图像
    if(m_showImage != nullptr){
        delete m_showImage;
        m_showImage = nullptr;
    }
    m_showImage = new QImage{m_StichedImage->data,m_StichedImage->cols,m_StichedImage->rows,m_StichedImage->cols,QImage::Format_Grayscale8};
    qDebug()<<"SepIncepReConsuctions Done...";
}

/*******************************************************缺陷检测部分*****************************************************************/
void ImageProcessThread::LockDefectMutexLocker()
{
    m_defectDetecionLocker.lock();
    m_keepDefectIncepLocker = true;
}

void ImageProcessThread::MutexLockerCallback()  //送往缺陷检测线程
{
    selfPtr->LockDefectMutexLocker();    //占有锁
    qDebug()<<"defect detection thread already processed all images....";
}

void ImageProcessThread::initilizeDefectInceptionThread()
{
    m_defectInceptionThread = new DefectDetectionThread{m_defectDetecionLocker,m_floatSepImages}; //创建对象
    //设置互斥锁回调函数
    m_defectInceptionThread->setMutexCallBack(MutexLockerCallback);
    LockDefectMutexLocker();

    m_defectInceptionThread->start();
    QString device_name = m_defectInceptionThread->GetDeviceName();
    qDebug()<<"Cuda Device Name: "<<device_name;
    qDebug()<<"defect detect thread create success...";
}

void ImageProcessThread::InvokeInceptionDefectThread()
{
    m_defectDetecionLocker.unlock();    //释放锁
    m_keepDefectIncepLocker = false;
}













#include "cudaHeader.cuh"

__global__ void roughGradient(float* imageData,float* gradientOut,float* directions,int pitch,int width,int height,int sep)
{
    int _u = threadIdx.x + blockIdx.x * blockDim.x * sep;  //需要调整
    int v = threadIdx.y + blockIdx.y * blockDim.y;
    //一个线程展开为4,内存访问的为对齐和合并的

    for(int i=0;i<sep;++i){
        int u = _u + blockDim.x * i;
        if((u < 1 || u > (width - 2)) || (v < 1 || v > (height - 2)))
            continue;
        int centerIdx = v * pitch + u;
        int topIdx = (v - 1) * pitch + u;
        int btmIdx = (v + 1) * pitch + u;
        float h_grad = -1 * imageData[topIdx - 1] + 0 * imageData[topIdx] + 1 * imageData[topIdx+1] + \
                       -2 * imageData[centerIdx - 1] + 0 * imageData[centerIdx] + 2 * imageData[centerIdx+1] +\
                       -1 * imageData[btmIdx - 1] + 0 * imageData[btmIdx] + 1 * imageData[btmIdx+1] ;

        float v_grad = -1 * imageData[topIdx - 1] + -2 * imageData[topIdx] + -1 * imageData[topIdx+1] + \
                       0 * imageData[centerIdx - 1] + 0 * imageData[centerIdx] + 0 * imageData[centerIdx+1] +\
                       1 * imageData[btmIdx - 1] + 2* imageData[btmIdx] + 1 * imageData[btmIdx+1];

        gradientOut[v * width + u] = sqrtf(__powf(fabs(h_grad),2.) + __powf(fabs(v_grad),2.));
        //printf("uu[2]: %f",uu[2]);
        //float direction = atanf(fabs(v_grad) / fabs(h_grad)) / 3.1416 * 180.f + 90.f;
        float direction = fabs(atan2f(v_grad , h_grad) / 3.1416 * 180.f);
        directions[v * width + u] = direction;
    }

}


__global__ void GetGridHisto(float* gradientValIn,float* gradientDirectionIn,float* histoOut,\
                             int sep,int bins,int width,int height,int gridSize,int gridX)
{
    int _x = threadIdx.x + blockIdx.x * blockDim.x;
    int _y = threadIdx.y + blockIdx.y * blockDim.y;     //_x,_y对应grid的位置
    int _u = _x * gridSize;
    int _v = _y * gridSize;
    //每个thread对应一个grid,thread当前的位置对应于block所处的位置
    if(_u > (width - gridSize - 1) || _v > (height - gridSize - 1)) //不足一个grid大小,不进行统计
        return;

    int rootIdx = _y * gridX + _x;  //一个block水平方向有blockDim.x个grid,一张图像水平方向有gridDim.x个block
    //_u和_v分别为各个grid左上点的坐标
    //非合并访问(...)
    float* root_address = histoOut + rootIdx * bins;    //定位到相应grid存放bin值的内存
    for(int i=_u;i<_u+gridSize;++i){    //无需范围判断
        for(int j=_v;j<_v+gridSize;++j){
            int idx = j * width + i;
            float gradient = gradientValIn[idx];
            float angle = gradientDirectionIn[idx];
            int floor = int((angle  - 1e-5) / sep);
            int ceil = floor + 1;

            float floor_wei = (ceil * sep - angle) / sep * gradient;
            float ceil_wei = (angle - floor * sep) / sep * gradient;
            if(ceil >= bins)
                ceil = 0;
            atomicAdd((root_address + floor),floor_wei);    //统计值
            atomicAdd((root_address + ceil),ceil_wei);
        }
    }
}


__global__ void Hog(float* gridHistoIn,float* hogOut,int gridPerRaw,int gridPerCol,int blockPerRaw,int bins)
{
    //一个线程负责一个block的组建,读取四个grid的histo写入hogOut相对应的位置,无写入冲突
    int _u = threadIdx.x + blockIdx.x * blockDim.x;
    int _v = threadIdx.y + blockIdx.y * blockDim.y;
    if(_u > (gridPerRaw - 2) || _v > (gridPerCol - 2))    //最后一个grid不计数
        return;
    //_u和_v对应于grid序号
    //取出两个起始头,每一个头往后为(18)位数据,
    float* header1 = gridHistoIn + (_u + _v * gridPerRaw) * bins;
    float* header2 = gridHistoIn + (_u + (_v+1) * gridPerRaw) * bins;   //下一行
    float* dstheader = hogOut + (_u + _v * blockPerRaw) * 4 * bins; //blockPerRaw为每行的cell个数-1
    //header1+18位 + header2+18位
    for(int i=0;i<bins * 2;++i){
        *(dstheader+i) = *(header1 + i);
        *(dstheader+i + 2 * bins) = *(header2 + i);
    }
    //normalize...,起点dstheader向后(4) * bins

    float squ_sum=0;
    for(int k=0;k<4*bins;++k)
    {
        squ_sum += __powf(fabs(*(dstheader+k)),2.);
    }

    squ_sum = sqrtf(squ_sum + 1e-7);
    for(int k=0;k<4*bins;++k)
    {
        *(dstheader+k) /= squ_sum;
    }
}

void roughGradientExcuter(float* imageData,float* gradientOut,float* directions,int pitch,int width,int height,int sep,dim3 grid,dim3 block)
{
    roughGradient<<<grid,block>>>(imageData,gradientOut,directions,pitch,width,height,sep);
}


void HOGGridHistogram(float* gradientValIn,float* gradientDirectionIn,float* histoOut,int sep,int bins,\
                      int width,int height,int gridSize,int gridX,dim3 grid,dim3 block)
{
    GetGridHisto<<<grid,block>>>(gradientValIn,gradientDirectionIn,histoOut,sep,bins,width,height,gridSize,gridX);
}

void HOGDecriptor(float* gridHistoIn,float* hogOut,int gridPerRaw,int gridPerCol,int blockPerRaw,int bins,dim3 grid,dim3 block)
{
    Hog<<<grid,block>>>(gridHistoIn,hogOut,gridPerRaw,gridPerCol,blockPerRaw,bins);
}

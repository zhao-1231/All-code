#ifndef CUDAHEADER_CUH
#define CUDAHEADER_CUH

#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>
#include <math.h>
void roughGradientExcuter(float* imageData,float* gradientOut,float* directions,int pitch,int width,int height,int sep,dim3,dim3);
void HOGGridHistogram(float* gradientValIn,float* gradientDirectionIn,float* histoOut,int sep,int bins,\
                      int width,int height,int gridSize,int gridX,dim3 grid,dim3 block);

void HOGDecriptor(float* gridHistoIn,float* hogOut,int gridPerRaw,int gridPerCol,int blockPerRaw,int bins,dim3 grid,dim3 block);

#endif // CUDAHEADER_CUH

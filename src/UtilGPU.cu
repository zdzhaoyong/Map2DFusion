/******************************************************************************

  This file is part of Map2DFusion.

  Copyright 2016 (c)  Yong Zhao <zd5945@126.com> http://www.zhaoyong.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/
#ifdef HAS_CUDA

#include "UtilGPU.cuh"
#include <stdio.h>
#include <base/time/Global_Timer.h>

__global__ void pyrDown(float4* in_data,int in_rows,int in_cols,float4* out_data,int out_rows,int out_cols)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (y < out_rows &&  x< out_cols)
    {
        float4* in_ptr=in_data+(y<<2)*out_cols+(x<<1);//in_data+y*2*in_cols+2*x=in_data[2*y][2*x]
        out_data[x+y*out_cols].x=0.25*(in_ptr[0].x+in_ptr[1].x+in_ptr[in_cols].x+in_ptr[in_cols+1].x);
        out_data[x+y*out_cols].y=0.25*(in_ptr[0].y+in_ptr[1].y+in_ptr[in_cols].y+in_ptr[in_cols+1].y);
        out_data[x+y*out_cols].z=0.25*(in_ptr[0].z+in_ptr[1].z+in_ptr[in_cols].z+in_ptr[in_cols+1].z);
        out_data[x+y*out_cols].w=0.25*(in_ptr[0].w+in_ptr[1].w+in_ptr[in_cols].w+in_ptr[in_cols+1].w);
    }
}

__global__ void pyrDownArray(float4** in_data,int in_rows,int in_cols,float4** out_datas,int out_rows,int out_cols,int num)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (y < out_rows &&  x< out_cols)
    {
        for(int i=0;i<num;i++)
        {
            float4* in_ptr=in_data[i]+(y<<2)*out_cols+(x<<1);//in_data+y*2*in_cols+2*x=in_data[2*y][2*x]
            float4* out_ptr=out_datas[i]+x+y*out_cols;
            (*out_ptr).x=0.25*(in_ptr[0].x+in_ptr[1].x+in_ptr[in_cols].x+in_ptr[in_cols+1].x);
            (*out_ptr).y=0.25*(in_ptr[0].y+in_ptr[1].y+in_ptr[in_cols].y+in_ptr[in_cols+1].y);
            (*out_ptr).z=0.25*(in_ptr[0].z+in_ptr[1].z+in_ptr[in_cols].z+in_ptr[in_cols+1].z);
            (*out_ptr).w=0.25*(in_ptr[0].w+in_ptr[1].w+in_ptr[in_cols].w+in_ptr[in_cols+1].w);
        }
    }
}

__global__ void pyrUp(float4* in_data,int in_rows,int in_cols,float4* out_data,int out_rows,int out_cols)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (y < out_rows &&  x< out_cols)
    {
        float4* in_ptr=in_data+(y>>1)*in_cols+(x<<1);//in_data+y*2*in_cols+2*x=in_data[2*y][2*x]
        out_data[x+y*out_cols].x=0.25*(in_ptr[0].x+in_ptr[1].x+in_ptr[in_cols].x+in_ptr[in_cols+1].x);
        out_data[x+y*out_cols].y=0.25*(in_ptr[0].y+in_ptr[1].y+in_ptr[in_cols].y+in_ptr[in_cols+1].y);
        out_data[x+y*out_cols].z=0.25*(in_ptr[0].z+in_ptr[1].z+in_ptr[in_cols].z+in_ptr[in_cols+1].z);
        out_data[x+y*out_cols].w=0.25*(in_ptr[0].w+in_ptr[1].w+in_ptr[in_cols].w+in_ptr[in_cols+1].w);
    }
}

__global__ void pyrUpArray(float4** in_data,int in_rows,int in_cols,float4** out_datas,int out_rows,int out_cols,int num)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (y < out_rows &&  x< out_cols)
    {
        for(int i=0;i<num;i++)
        {
            float4* in_ptr=in_data[i]+(y<<2)*out_cols+(x<<1);//in_data+y*2*in_cols+2*x=in_data[2*y][2*x]
            float4* out_ptr=out_datas[i]+x+y*out_cols;
            (*out_ptr).x=0.25*(in_ptr[0].x+in_ptr[1].x+in_ptr[in_cols].x+in_ptr[in_cols+1].x);
            (*out_ptr).y=0.25*(in_ptr[0].y+in_ptr[1].y+in_ptr[in_cols].y+in_ptr[in_cols+1].y);
            (*out_ptr).z=0.25*(in_ptr[0].z+in_ptr[1].z+in_ptr[in_cols].z+in_ptr[in_cols+1].z);
            (*out_ptr).w=0.25*(in_ptr[0].w+in_ptr[1].w+in_ptr[in_cols].w+in_ptr[in_cols+1].w);
        }
    }
}

template <class T>
__global__ void warpPerspectiveKernel(int in_rows,int in_cols,T* in_data,
                                      int out_rows,int out_cols,T* out_data,
                                      float* inv,T defVar)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (y < out_rows &&  x< out_cols)
    {
        float srcX=inv[0]*x+inv[1]*y+inv[2];
        float srcY=inv[3]*x+inv[4]*y+inv[5];
        float srcW=inv[6]*x+inv[7]*y+inv[8];
        srcW=1./srcW;srcX*=srcW;srcY*=srcW;
        if(srcX<in_cols&&srcX>=0&&srcY<in_rows&&srcY>=0)
        {
            out_data[x+y*out_cols]=in_data[(int)srcX+((int)srcY)*in_cols];
        }
        else
        {
            out_data[x+y*out_cols]=defVar;
        }
    }
}

template <class T>
bool operate<T>::warpPerspectiveCaller(int in_rows,int in_cols,T* in_data,
                           int out_rows,int out_cols,T* out_data,
                           float* inv,T defVar)
{
    T* in_dataGPU;
    T* out_dataGPU;
    float* invGPU;
    cudaMalloc((void**) &in_dataGPU, in_cols*in_rows*sizeof(T));
    cudaMalloc((void**) &out_dataGPU,out_cols*out_rows*sizeof(T));
    cudaMalloc((void**) &invGPU,9*sizeof(float));
    cudaMemcpy(in_dataGPU,in_data,in_cols*in_rows*sizeof(T),cudaMemcpyHostToDevice);
    cudaMemcpy(invGPU,inv,9*sizeof(float),cudaMemcpyHostToDevice);

    dim3 threads(32,32);
        dim3 grid(divUp(out_cols, threads.x), divUp(out_rows, threads.y));
//        dim3 grid(20,20);

    pi::timer.enter("warpPerspectiveKernel");
    warpPerspectiveKernel<T><<<grid,threads>>>(in_rows,in_cols,in_dataGPU,
                                               out_rows,out_cols,out_dataGPU,
                                               invGPU,defVar);
    pi::timer.leave("warpPerspectiveKernel");

    cudaMemcpy(out_data,out_dataGPU,out_cols*out_rows*sizeof(T),cudaMemcpyDeviceToHost);
    cudaFree(in_dataGPU);cudaFree(out_dataGPU);cudaFree(invGPU);
    return true;
}

bool warpPerspective_uchar1(int in_rows,int in_cols,uchar1* in_data,
                            int out_rows,int out_cols,uchar1* out_data,
                            float* inv,uchar1 defVar)
{
    return operate<uchar1>::warpPerspectiveCaller(in_rows,in_cols,in_data,
                                                  out_rows,out_cols,out_data,
                                                  inv,defVar);
}
//bool warpPerspective_uchar2();
bool warpPerspective_uchar3(int in_rows,int in_cols,uchar3* in_data,
                            int out_rows,int out_cols,uchar3* out_data,
                            float* inv,uchar3 defVar)
{
    return operate<uchar3>::warpPerspectiveCaller(in_rows,in_cols,in_data,
                                                  out_rows,out_cols,out_data,
                                                  inv,defVar);
}

bool warpPerspective_uchar4(int in_rows,int in_cols,uchar4* in_data,
                            int out_rows,int out_cols,uchar4* out_data,
                            float* inv,uchar4 defVar)
{
    return operate<uchar4>::warpPerspectiveCaller(in_rows,in_cols,in_data,
                                                  out_rows,out_cols,out_data,
                                                  inv,defVar);
}


__global__ void renderFrameKernel(int in_rows,int in_cols,uchar3* in_data,//image in
                                  int out_rows,int out_cols,uchar4* out_data,
                                  bool fresh,uchar4 defVar,//image out
                                  float* inv,int centerX,int centerY//relations
                                  )
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if(y<out_rows&&x<out_cols)
    {
        int idxOut=x+y*out_cols;

        // find source location
        float srcX=inv[0]*x+inv[1]*y+inv[2];
        float srcY=inv[3]*x+inv[4]*y+inv[5];
        float srcW=inv[6]*x+inv[7]*y+inv[8];
        srcW=1./srcW;srcX*=srcW;srcY*=srcW;

        if(fresh) //warp
        {
            if(srcX<in_cols&&srcX>=0&&srcY<in_rows&&srcY>=0)
            {
                uchar4* ptrOut=out_data+idxOut;
                *((uchar3*)ptrOut)=in_data[(int)srcX+((int)srcY)*in_cols];
                // compute weight
                {
                    //image weight
                    float difX=srcX-in_rows*0.5;
                    float difY=srcY-in_cols*0.5;
                    srcW=1000*(difX*difX+difY*difY)/(in_rows*in_rows+in_cols*in_cols);
                    if(srcW<1) srcW=1;
                    //center weight
                }
                ptrOut->w=srcW;
            }
            else
            {
                out_data[idxOut]=defVar;
            }

        }
        else if(srcX<in_cols&&srcX>=0&&srcY<in_rows&&srcY>=0)// blender
        {
            uchar4* ptrOut=out_data+idxOut;
            // compute weight
            {
                //image weight
                float difX=srcX-in_rows*0.5;
                float difY=srcY-in_cols*0.5;
                srcW=1000*(0.25-(difX*difX+difY*difY)/(in_rows*in_rows+in_cols*in_cols));
                if(srcW<1) srcW=1;
                //center weight
            }
            if(ptrOut->w<srcW)
            {
                ptrOut->w=srcW;
                uchar3* ptrIn =in_data +(int)srcX+((int)srcY)*in_cols;
                *((uchar3*)ptrOut)=*ptrIn;
            }
        }
    }
}

__global__ void renderFramesKernel(int in_rows,int in_cols,uchar3* in_data,//image in
                                   int out_rows,int out_cols,uchar4** out_datas,
                                   bool* freshs,uchar4 defVar,//image out
                                   float* invs,int* centers,int eleNum//relations
                                  )
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if(y<out_rows&&x<out_cols)
    {
        for(int i=0;i<eleNum;i++)
        {
            uchar4* out_data=out_datas[i];
            bool    fresh=freshs[i];
            float*  inv=invs+9*i;

            int idxOut=x+y*out_cols;
            uchar4* ptrOut=out_data+idxOut;

            // find source location
            float srcX=inv[0]*x+inv[1]*y+inv[2];
            float srcY=inv[3]*x+inv[4]*y+inv[5];
            float srcW=inv[6]*x+inv[7]*y+inv[8];
            srcW=1./srcW;srcX*=srcW;srcY*=srcW;

            if(srcX<in_cols&&srcX>=0&&srcY<in_rows&&srcY>=0)
            {
                // compute weight
                {
                    //image weight
                    float difX=srcX-in_rows*0.5;
                    float difY=srcY-in_cols*0.5;
                    srcW=(0.25-(difX*difX+difY*difY)/(in_rows*in_rows+in_cols*in_cols));//0~0.25
                    //center weight
                    if(1)
                    {
                        difX=centers[i*2]-x;
                        difY=centers[i*2+1]-y;
                        srcW=5e4*srcW/sqrt(difX*difX+difY*difY+1);
                    }
                    else
                        srcW=1000*srcW;
                    if(srcW<1) srcW=1;
                    else if(srcW>255) srcW=255;
                }
                if(fresh)
                {
                    *((uchar3*)ptrOut)=in_data[(int)srcX+((int)srcY)*in_cols];

                    ptrOut->w=srcW;
                }
                else// blender
                {
                    if(ptrOut->w<srcW)
                    {
                        ptrOut->w=srcW;
                        uchar3* ptrIn =in_data +(int)srcX+((int)srcY)*in_cols;
                        *((uchar3*)ptrOut)=*ptrIn;
                    }
                }
            }
            else if(fresh)
            {
                *ptrOut=defVar;
            }
        }
    }
}

__global__ void renderFramesKernel(int in_rows,int in_cols,uchar3* in_data,//image in
                                   int out_rows,int out_cols,float4** out_datas,
                                   bool* freshs,float4 defVar,//image out
                                   float* invs,int* centers,int eleNum//relations
                                  )
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if(y<out_rows&&x<out_cols)
    {
        for(int i=0;i<eleNum;i++)
        {
            float4* out_data=out_datas[i];
            bool    fresh=freshs[i];
            float*  inv=invs+9*i;

            int idxOut=x+y*out_cols;
            float4* ptrOut=out_data+idxOut;

            // find source location
            float srcX=inv[0]*x+inv[1]*y+inv[2];
            float srcY=inv[3]*x+inv[4]*y+inv[5];
            float srcW=inv[6]*x+inv[7]*y+inv[8];
            srcW=1./srcW;srcX*=srcW;srcY*=srcW;

            if(srcX<in_cols&&srcX>=0&&srcY<in_rows&&srcY>=0)
            {
                // compute weight
                {
                    if(0)
                    {
                        //image weight //1-sqrt(dis)/dis_max;
                        float difX=srcX/(float)in_cols-0.5;
                        float difY=srcY/(float)in_rows-0.5;
                        srcW=0.5-(difX*difX+difY*difY);//0~0.25
                    }
                    else srcW=1;

                    //center weight
                    if(1)
                    {
                        float difX=centers[i*2]-x;
                        float difY=centers[i*2+1]-y;
                        srcW=1e5*srcW/(difX*difX+difY*difY+1000);
                    }
                }
                if(fresh)
                {
                    uchar3* ptrIn =in_data +(int)srcX+((int)srcY)*in_cols;
                    ptrOut->x=ptrIn->x*0.00392f;//~=/256
                    ptrOut->y=ptrIn->y*0.00392f;
                    ptrOut->z=ptrIn->z*0.00392f;
                    ptrOut->w=srcW;
                }
                else if(ptrOut->w<=srcW)
                {
                    uchar3* ptrIn =in_data +(int)srcX+((int)srcY)*in_cols;
                    float   sumweightInv=1./(ptrOut->w*2+srcW);
                    ptrOut->x=(ptrOut->x*ptrOut->w*2+ptrIn->x*0.00392f*srcW)*sumweightInv;//~=/256
                    ptrOut->y=(ptrOut->y*ptrOut->w*2+ptrIn->y*0.00392f*srcW)*sumweightInv;
                    ptrOut->z=(ptrOut->z*ptrOut->w*2+ptrIn->z*0.00392f*srcW)*sumweightInv;
                    ptrOut->w=srcW;
                }
            }
            else if(fresh)
            {
                *ptrOut=defVar;
            }
        }
    }
}

bool renderFrameCaller(CudaImage<uchar3>& rgbIn,CudaImage<uchar4>& ele,
                       float* inv,int centerX,int centerY)
{
    float* invGPU;
    checkCudaErrors(cudaMalloc((void**) &invGPU,9*sizeof(float)));
    checkCudaErrors(cudaMemcpy(invGPU,inv,9*sizeof(float),cudaMemcpyHostToDevice));
    dim3 threads(32,32);
    uchar4 defVar;
    defVar.x=defVar.y=defVar.z=defVar.w=0;
    dim3 grid(divUp(ele.cols, threads.x), divUp(ele.rows, threads.y));
    renderFrameKernel<<<grid,threads>>>(rgbIn.rows,rgbIn.cols,rgbIn.data,
                                        ele.rows,ele.cols,ele.data,
                                        ele.fresh,defVar,invGPU,centerX,centerY);
    checkCudaErrors(cudaFree(invGPU));
    return true;
}


bool renderFramesCaller(CudaImage<uchar3>& rgbIn,int out_rows,int out_cols,
                        uchar4** out_datas,bool* freshs,
                       float* invs,int* centers,int eleNum)
{
    float* invGPU;
    uchar4** outDataGPU;
    bool*  freshesGPU;
    int*   centersGPU;

    checkCudaErrors(cudaMalloc((void**) &invGPU,9*sizeof(float)*eleNum));
    checkCudaErrors(cudaMalloc((void**) &outDataGPU,sizeof(uchar4*)*eleNum));
    checkCudaErrors(cudaMalloc((void**) &freshesGPU,sizeof(bool)*eleNum));
    checkCudaErrors(cudaMalloc((void**) &centersGPU,2*sizeof(int)*eleNum));
    checkCudaErrors(cudaMemcpy(invGPU,invs,9*sizeof(float)*eleNum,cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(outDataGPU,out_datas,sizeof(uchar4*)*eleNum,cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(freshesGPU,freshs,sizeof(bool)*eleNum,cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(centersGPU,centers,2*sizeof(int)*eleNum,cudaMemcpyHostToDevice));
    dim3 threads(32,32);
    uchar4 defVar;
    defVar.x=defVar.y=defVar.z=defVar.w=0;
    dim3 grid(divUp(out_cols, threads.x), divUp(out_rows, threads.y));
    renderFramesKernel<<<grid,threads>>>(rgbIn.rows,rgbIn.cols,rgbIn.data,
                                        out_rows,out_cols,outDataGPU,freshesGPU,
                                        defVar,invGPU,centersGPU,eleNum);
    checkCudaErrors(cudaFree(invGPU));
    checkCudaErrors(cudaFree(outDataGPU));
    checkCudaErrors(cudaFree(freshesGPU));
    checkCudaErrors(cudaFree(centersGPU));
    return true;
}

bool renderFramesCaller(CudaImage<uchar3>& rgbIn,int out_rows,int out_cols,
                        float4** out_datas,bool* freshs,
                       float* invs,int* centers,int eleNum)
{
    float* invGPU;
    float4** outDataGPU;
    bool*  freshesGPU;
    int*   centersGPU;

    checkCudaErrors(cudaMalloc((void**) &invGPU,9*sizeof(float)*eleNum));
    checkCudaErrors(cudaMalloc((void**) &outDataGPU,sizeof(float4*)*eleNum));
    checkCudaErrors(cudaMalloc((void**) &freshesGPU,sizeof(bool)*eleNum));
    checkCudaErrors(cudaMalloc((void**) &centersGPU,2*sizeof(int)*eleNum));
    checkCudaErrors(cudaMemcpy(invGPU,invs,9*sizeof(float)*eleNum,cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(outDataGPU,out_datas,sizeof(float4*)*eleNum,cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(freshesGPU,freshs,sizeof(bool)*eleNum,cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(centersGPU,centers,2*sizeof(int)*eleNum,cudaMemcpyHostToDevice));
    dim3 threads(32,32);
    float4 defVar;
    defVar.x=defVar.y=defVar.z=defVar.w=0;
    dim3 grid(divUp(out_cols, threads.x), divUp(out_rows, threads.y));
    renderFramesKernel<<<grid,threads>>>(rgbIn.rows,rgbIn.cols,rgbIn.data,
                                        out_rows,out_cols,outDataGPU,freshesGPU,
                                        defVar,invGPU,centersGPU,eleNum);
    checkCudaErrors(cudaFree(invGPU));
    checkCudaErrors(cudaFree(outDataGPU));
    checkCudaErrors(cudaFree(freshesGPU));
    checkCudaErrors(cudaFree(centersGPU));
    return true;
}

#endif

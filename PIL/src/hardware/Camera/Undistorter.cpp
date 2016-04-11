#include "Undistorter.h"

#include "base/Svar/Svar.h"
#include "base/debug/debug_config.h"

#include <sstream>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <omp.h>

//图像的像素直接提取
#define        _I(x,y) p_img[(c)*((int)(y)*(width_in)+(int)(x))]
//亚像素级灰度值
#define        _IF(x,y) (((int)(x+1)-(x))*((int)(y+1)-(y))*_I((image),(int)(x),(int)(y)) + ((int)(x+1)-(x))*((y)-(int)(y))*_I((image),(int)(x),(int)(y+1)) + ((x)-(int)(x))*((int)(y+1)-(y))*_I((image),(int)(x+1),(int)(y)) + ((x)-(int)(x))*((y)-(int)(y))*_I((image),(int)(x+1),(int)(y+1)) )//插值后的像素值(IN表示interpolation),x、y可以为小数

using namespace std;

template <int Size>
struct Byte
{
    unsigned char data[Size];
};

typedef Byte<3> rgb;


Undistorter::Undistorter(std::string config_file)
{
    valid = false;

    camera_in   = NULL;
    camera_out  = NULL;

    remapX = NULL;
    remapY = NULL;
    remapIdx  = NULL;
    remapCoef = NULL;

    Svar config;
    config.ParseFile(config_file);

    camera_in  = GetCameraFromFile(config.GetString("CameraIn", "in.cam"));
    camera_out = GetCameraFromFile(config.GetString("CameraOut","out.cam"));

    prepareReMap();

}

Undistorter::~Undistorter()
{
    if( camera_in != NULL )     delete camera_in;
    if( camera_out != NULL )    delete camera_out;

    if( remapX != NULL )        delete[] remapX;
    if( remapY != NULL )        delete[] remapY;

    if( remapIdx != NULL )      delete[] remapIdx;
    if( remapCoef != NULL)      delete[] remapCoef;
}


bool Undistorter::prepareReMap()
{
    // check camera model
    if( !(camera_in && camera_out) )
    {
        valid = false;
        dbg_pe("Undistorter does not get vallid camera.");
        return false;
    }

    width_in   = camera_in->Width();
    width_out  = camera_out->Width();
    height_in  = camera_in->Height();
    height_out = camera_out->Height();


    valid = camera_in->isValid() && camera_out->isValid();
    if( valid )
    {
        // Prepare remap
        cout << "Undistorter:\n";
        cout << "    Camera IN : " << camera_in->info() << endl;
        cout << "    Camera OUT: " << camera_out->info() << endl << endl;

        remapX    = new float[width_out * height_out];
        remapY    = new float[width_out * height_out];
        remapFast = new int[width_out * height_out];

        remapIdx  = new int  [width_out*height_out*4];
        remapCoef = new float[width_out*height_out*4];

        for(int y=0; y<height_out; y++)
            for(int x=0; x<width_out; x++)
            {
                int i = y*width_out+x;

                Point2D tmp=camera_out->UnProject(x,y);
                tmp = camera_in->Project(tmp[0],tmp[1]);

                remapX[i]    = tmp[0];
                remapY[i]    = tmp[1];
                remapFast[i] = (int)tmp[0]+width_in*(int)tmp[1];

                if(tmp[0]<0 || tmp[1]<0 ||
                   tmp[0]>=width_in||tmp[1]>=height_in)
                {
                    remapX[y*width_out+x]    = -1;
                    remapFast[y*width_out+x] = -1;
//                    cerr<<"Out of image: dest:["<<x<<","<<y
//                       <<"], src:["<<tmp<<"], plane:["<<camera_out->UnProject(x,y)<<"]\n";
                }

                // calculate fast bi-linear interpolation indices & coefficients
                {
                    float xx = remapX[i];
                    float yy = remapY[i];

                    if( xx < 0.0 ) continue;

                    // get integer and rational parts
                    int xxi = xx;
                    int yyi = yy;
                    xx -= xxi;
                    yy -= yyi;
                    float xxyy = xx*yy;

                    remapIdx[i*4+0]  = yyi*width_in + xxi;
                    remapIdx[i*4+1]  = yyi*width_in + xxi + 1;
                    remapIdx[i*4+2]  = (yyi+1)*width_in + xxi;
                    remapIdx[i*4+3]  = (yyi+1)*width_in + xxi + 1;

                    remapCoef[i*4+0] = 1-xx-yy+xxyy;
                    remapCoef[i*4+1] = xx-xxyy;
                    remapCoef[i*4+2] = yy-xxyy;
                    remapCoef[i*4+3] = xxyy;
                }
            }
        //cout<<"Remap prepared.\n";
    }
    else
    {
        dbg_pe("Undistorter is not valid!");
    }

    return valid;
}

//Undistorting fast, no interpolate (bilinear) is used
bool Undistorter::undistortFast(const cv::Mat& image, cv::Mat& result)
{
    if (!valid)
    {
        dbg_pe("Undistorter is not valid! Not undistorting.\n");
        result = image;
        return false;
    }

    if (image.rows != height_in || image.cols != width_in)
    {
        dbg_pe("input image size differs from expected input size! Not undistorting.\n");
        result = image;
        return false;
    }


    int wh=width_out*height_out;
    int c=image.channels();

    result=image.zeros(height_out,width_out,image.type());

    if(c==1)
    {
        Byte<1>* p_out=(Byte<1>*)result.data;
        Byte<1>* p_img=(Byte<1>*)image.data;
        #pragma omp parallel for
        for(int i=0;i<wh;i++)
        {
            if(remapFast[i]>0)
            {
               p_out[i] = p_img[remapFast[i]];
            }
        }
    }
    else if(c==3)
    {
        Byte<3>* p_out=(Byte<3>*)result.data;
        Byte<3>* p_img=(Byte<3>*)image.data;
        #pragma omp parallel for
        for(int i=0;i<wh;i++)
        {
            p_out[i] = p_img[remapFast[i]];
        }
    }
    else
    {
        Byte<1>* p_out=(Byte<1>*)result.data;
        Byte<1>* p_img=(Byte<1>*)image.data;
        #pragma omp parallel for
        for(int i=0;i<wh;i++)
        {
            if(remapX[i]>0)
            {
                for(int j=0;j<c;j++)
                    p_out[i*c+j]=p_img[remapFast[i]*c+j];
            }
        }
    }

    return true;
}

//Undistorting bilinear interpolation
bool Undistorter::undistort(const cv::Mat& image, cv::Mat& result)
{
    if (!valid)
    {
        result = image;
        return false;
    }

    if (image.rows != height_in || image.cols != width_in)
    {
        dbg_pe("input image size differs from expected input size! Not undistorting.\n");
        result = image;
        return false;
    }


    int wh=width_out*height_out;
    int c=image.channels();

    result=image.zeros(height_out,width_out,image.type());

    if(c==1)
    {
        uchar* p_out=(uchar*)result.data;
        uchar* p_img=(uchar*)image.data;

        int   *pIdx  = remapIdx;
        float *pCoef = remapCoef;

//        #pragma omp parallel for
        for(int i = 0; i<wh; i++)
        {
            // get interp. values
            float xx = remapX[i];

            if(xx<0)
                p_out[i] = 0;
            else
            {
                p_out[i] = p_img[pIdx[0]]*pCoef[0] +
                           p_img[pIdx[1]]*pCoef[1] +
                           p_img[pIdx[2]]*pCoef[2] +
                           p_img[pIdx[3]]*pCoef[3];
            }

            pIdx  += 4;
            pCoef += 4;
        }
    }
    else
    {
        uchar* p_out = (uchar*)result.data;
        uchar* p_img = (uchar*)image.data;

        int   *pIdx  = remapIdx;
        float *pCoef = remapCoef;

//        #pragma omp parallel for
        for(int i=0; i<wh; i++)
        {
            if(remapX[i]>0)
            {
                for(int j=0; j<c; j++)
                    p_out[i*3+j] = p_img[pIdx[0]*c+j]*pCoef[0] +
                                   p_img[pIdx[1]*c+j]*pCoef[1] +
                                   p_img[pIdx[2]*c+j]*pCoef[2] +
                                   p_img[pIdx[3]*c+j]*pCoef[3];
            }

            pIdx  += 4;
            pCoef += 4;
        }
    }

    return true;
}

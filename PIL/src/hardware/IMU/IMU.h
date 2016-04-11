#ifndef IMU_H
#define IMU_H

#ifdef HAS_TOON
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <deque>
#include <fstream>

#include <base/types/SE3.h>
#include <base/system/thread/ThreadBase.h>

#include "AHRS_mini.h"


namespace pi {

using namespace std;

typedef u_int64_t u_int64;


template <int CHANNEL=3,int ORDER=1>
class Filter
{
public:
    Filter()
    {
        reset();
    }

    void reset()
    {
        //        b[0]=0.0007; b[1]=0.0021;  b[2]=0.0021; b[3]=0.0007;
        //        a[0]=1.0000; a[1]=-2.6236; a[2]=2.3147; a[3]=-0.6855;
        b[0]=0.998774318891425;
        b[1]=-0.998774318891425;
        a[0]=1.000000000000000;
        a[1]=-0.997548637782850;

        for(int i=0;i<CHANNEL-1;i++)
            for(int j=0;j<ORDER;j++)
            {
                in_buffer[i][j]=out_buffer[i][j]=0.;
            }
    }

    double getVal(double val,int channel=0)
    {
        for (int i=0;i<ORDER;i++) {in_buffer[channel][ORDER-i]=in_buffer[channel][ORDER-i-1];}
        in_buffer[channel][0]=val;

        double xout=0.0;

        for (int j=0;j<ORDER+1;j++)
        xout=xout+b[j]*in_buffer[channel][j];
        for (int j=0;j<ORDER;j++)
        xout=xout-a[j+1]*out_buffer[channel][j];

        for (int i=0;i<ORDER-1;i++) {out_buffer[channel][ORDER-i-1]=out_buffer[channel][ORDER-i-2];}
        out_buffer[channel][0]=xout;
        return xout;
    }

    pi::Point3d getVal(pi::Point3d in)
    {
        return Point3d(getVal(in[0],0),getVal(in[1],1),getVal(in[2],2));
    }
protected:
    double a[ORDER+1],b[ORDER+1];
    double in_buffer[CHANNEL][ORDER+1],out_buffer[CHANNEL][ORDER+1];

};

class AHRS_Drift
{
public:
    AHRS_Drift()
    {
        SetFromBoundary(16384,-16384,16384,-16384,16384,-16384);
    }

    AHRS_Drift(string filename)
    {
        ifstream file(filename.c_str());
        if(file.is_open())
        {
            file>>maxAx>>minAx>>maxAy>>minAy>>maxAz>>minAz;
            SetFromBoundary();
        }
        else
         SetFromBoundary(16384,-16384,16384,-16384,16384,-16384);
    }

    void SetFromBoundary()
    {
        Xa=32768.0/(maxAx-minAx); Xb=(minAx+maxAx)/(minAx-maxAx);
        Ya=32768.0/(maxAy-minAy); Xb=(minAy+maxAy)/(minAy-maxAy);
        Za=32768.0/(maxAz-minAz); Xb=(minAz+maxAz)/(minAz-maxAz);
    }
    void SetFromBoundary(double maxX,double minX,double maxY,double minY,double maxZ,double minZ)
    {
        maxAx=maxX;minAx=minX;maxAy=maxY;minAy=minY;maxAz=maxZ;minAz=minZ;
        SetFromBoundary();
    }
    void SetFromGradient()
    {
        maxAx=(16384.0-Xb)/Xa; minAx=(-16384.0-Xb)/Xa;
        maxAy=(16384.0-Yb)/Ya; minAy=(-16384.0-Yb)/Ya;
        maxAz=(16384.0-Zb)/Za; minAz=(-16384.0-Zb)/Za;
    }

    void SetFromGradient(double xa,double xb,double ya,double yb,double za,double zb)
    {
        Xa=xa;Xb=xb;Ya=ya;Yb=yb;Za=za;Zb=zb;
        SetFromGradient();
    }
    float getAx(float Ax){return Xa*Ax+Xb;}
    float getAy(float Ay){return Ya*Ay+Yb;}
    float getAz(float Az){return Za*Az+Zb;}

    void CalibrateFromeVector(vector<Point3d > &measure,vector<Point3d > &shouldB);
    void SaveCalibration2File(string filename)
    {
        ofstream driftfile(filename.c_str());
        cout<<maxAx<<endl<<minAx<<endl
                   <<maxAy<<endl<<minAy<<endl
                     <<maxAz<<endl<<minAz<<endl;
        driftfile<<maxAx<<endl<<minAx<<endl
                   <<maxAy<<endl<<minAy<<endl
                     <<maxAz<<endl<<minAz<<endl;
    }
    void PrintParaments()
    {
        printf("The paraments of drift:\nXa=%f,Xb=%f,Ya=%f,Yb=%f,Za=%f,Zb=%f\n",Xa,Xb,Ya,Yb,Za,Zb);
        printf("maxAx=%f,minAx=%f,maxAy=%f,minAy=%f,maxAz=%f,minAz=%f\n",maxAx,minAx,maxAy,minAy,maxAz,minAz);
    }
private:
    void LSM(vector<double> &x,vector<double> &y,double &a,double &b)
    {
        unsigned int n=x.size();
        if(n!=y.size()) {cout<<"LSM:not the same size!";return;}

        double x_avrg,y_avrg,x_sum=0,y_sum=0,xy_sum=0,x2_sum=0;
        for(unsigned int i=0;i<n;i++)
        {
            x_sum+=x[i];
            y_sum+=y[i];
            xy_sum+=x[i]*y[i];
            x2_sum+=x[i]*x[i];
        }
        x_avrg=x_sum/n;
        y_avrg=y_sum/n;
        a=(n*xy_sum-x_sum*y_sum)/(n*x2_sum-x_sum*x_sum);
        b=y_avrg-a*x_avrg;

        cout<<"LSMData: x:";
        for(int i=0;i<x.size();i++) cout<<x[i]<<",";
        cout<<" y:";
        for(int i=0;i<y.size();i++) cout<<y[i]<<",";
        cout<<"\nAnswer: a="<<a<<" b="<<b<<endl;
    }

    double maxAx,minAx,maxAy,minAy,maxAz,minAz;
    double Xa,Xb,Ya,Yb,Za,Zb;
};

class IMU_Frame
{
public:
    IMU_Frame(){}
    IMU_Frame(AHRS_Frame Frame)
    {
        frame=Frame;
        unfilt_trans=filted_v=v=rv=Point3d(0,0,0);

    }
    bool isValid();
    void UpdateRotation();
    void UpdateRotation(double yaw,double pitch,double roll);
    void SaveProcess2File();
    Point3d AbsoluteAccelarate();
    AHRS_Frame  frame;
    Point3d   v,rv,acc,filted_v,unfilt_trans; //velocity
    pi::SE3<>       pose;
};

class SpillHandle
{
  public:
    SpillHandle()
    {
        NeedEdit[0]=false;
        NeedEdit[1]=false;
        NeedEdit[2]=false;
        lastVal[0]=0;
        lastVal[1]=0;
        lastVal[2]=0;
    }
    void GetTrueVal(int &ax,int &ay,int &az)
    {
        GetTrueValX(ax,0);
        GetTrueValX(ay,1);
        GetTrueValZ(az);
    }
    void GetTrueValX(int &val,int n)
    {
        if(abs(val)<3000) NeedEdit[n]=false;
        else
        if(abs(val-lastVal[n])>16384) NeedEdit[n]=!NeedEdit[n];
        lastVal[n]=val;
        if(NeedEdit[n])
        {
            if(val<-16384) val=val+65536;
            else           val=val-65536;
        }
    }
    void GetTrueValY(int &val,int n=1)
    {
        if(abs(val)<8192) NeedEdit[n]=false;
        else
        if(abs(val-lastVal[n])>8192) NeedEdit[n]=!NeedEdit[n];
        lastVal[n]=val;
        if(NeedEdit[n])
        {
            if(val<-8192) val=val+16384;
            else          val=val-16384;
        }
    }
    void GetTrueValZ(int &val,int n=2)
    {
        if(abs(val+16384)<16384) NeedEdit[n]=false;
        else
            if(abs(val-lastVal[n])>16384) NeedEdit[n]=!NeedEdit[n];
            lastVal[n]=val;
            if(NeedEdit[n])
            {
                if(val<-16384) val=val+65536;
                else           val=val-65536;
            }
    }

  protected:
    bool NeedEdit[3];
    int lastVal[3];
};

class IMU:public Thread
{
public:
    IMU();
    ~IMU(){stop();}
    IMU(pi::SE3<> initialPose);

    void setPose(pi::SE3<> pose,Point3d v,u_int64 time);
    pi::SE3<> ComputePose(u_int64 time);

    virtual void run();
    virtual void stop(){ahrs.stop();insertFrame=false;pthread_join(readThread, NULL);}

    void CalibrateIMU(int sampleNum=6);

private:
    pi::SE3<> ComputeFrame(IMU_Frame &lastFrame,IMU_Frame &frame2Compute);
    pi::SE3<> ComputeFrame(IMU_Frame &lastFrame);

public:
    AHRS_Mini           ahrs;
    deque<IMU_Frame>    data;
    u_int64             firstFrameTime;
    AHRS_Drift          drift;
    bool                isCalibrating,insertFrame;
    IMU_Frame           CurrentFrame;
    SpillHandle         spill_handle;

private:
    pthread_t           readThread;
};


template <class ValType>
TooN::Matrix<3,3,double> rebuildRotation(const ValType &m_yaw,const ValType &m_pitch,const ValType &m_roll);

TooN::SO3<> rebuildSO3(double yaw,double pitch,double roll);

}
#endif//HAS_TOON?

#endif // IMU_H

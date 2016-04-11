#include "IMU.h"

#ifdef HAS_TOON
//#include <base/utils.h>
#include <base/Svar/Svar_Inc.h>

#define earth_g 9.80665
#define deg2rad  0.017453292519943

#define DebugMSG 0
#if DebugMSG
#define msg(a) cout<<a<<endl;
#else
#define msg(a)
#endif

namespace pi{

static Filter<3,3> filter,filterpose;

void AHRS_Drift::CalibrateFromeVector(vector<Point3d > &measure,vector<Point3d > &shouldB)
{
    unsigned int n=measure.size();
    if(n!=shouldB.size()) {cout<<"Calibrator:not the same size!";return;}
    Point3d A,B;
    for(unsigned int j=0;j<3;j++)
    {
        vector<double> x,y;
        for(unsigned int i=0;i<n;i++)
        {
            x.push_back(measure[i][j]);
            y.push_back(shouldB[i][j]);
        }
        LSM(x,y,A[j],B[j]);
    }
    SetFromGradient(A[0],B[0],A[1],B[1],A[2],B[2]);
    if(svar.GetInt("SaveCalibrateData",1))
    {
        ofstream calib_data("calib_data.txt");
        for(int i=0;i<n;i++)
            calib_data<<measure[i][0]<<" "<<measure[i][1]<<" "<<measure[i][2]<<" "
                                    <<shouldB[i][0]<<" "<<shouldB[i][1]<<" "<<shouldB[i][2]<<"\n";
    }
}

void IMU_Frame::UpdateRotation(double yaw,double pitch,double roll)
{
    if(0)
    {
        TooN::Matrix<3,3,double> R=rebuildRotation(yaw,pitch,roll);
        pose.get_rotation().fromMatrixUnsafe(R);
        cout<<R<<endl;
    }
    else
    {
        pose.get_rotation().FromEuler(pitch*deg2rad,yaw*deg2rad,roll*deg2rad);
//        pose.get_rotation()=pose.get_rotation().inv();
    }
}

void IMU_Frame::UpdateRotation()
{
    UpdateRotation(frame.yaw,frame.pitch,frame.roll);
}

Point3d IMU_Frame::AbsoluteAccelarate()
{
    double dd=16384.0/earth_g;
    acc=Point3d(frame.Ax/dd,frame.Ay/dd,frame.Az/dd);
    msg("acc="<<acc);
    acc=pose.get_rotation()*acc+Point3d(0,0,earth_g);
    acc=-acc;
//    if(acc[2]<-20) acc[2]+=earth_g*2;
    msg("pose.get_rotation.inverce="<<pose.get_rotation().inverse());
    msg("acc="<<acc);
    if(svar.GetInt("UseAccFilter",0))
    {
        static Filter<3> acc_filter;
        acc_filter.getVal(acc);
    }
    return acc;
}

IMU::IMU():drift("drift.txt")
{
    std::string     port;
    port = "/dev/ttyUSB0";
    //drift.SetFromBoundary(16518,-16138,16321,-16302,14587,-18488);
    isCalibrating=false;
    insertFrame=true;

    ahrs.setUART(port.c_str());
    ahrs.start();
    start();
    while(data.size()==0)
        sleep(1);
    CurrentFrame=data.front();
    data.pop_front();
    //set the pose to be the first AHRS frame rotation
    CurrentFrame.UpdateRotation();
}

Point3d earth_g2IMU(Point3d Rot)
{
    TooN::Matrix<3,3,double> R=rebuildRotation(Rot[2],Rot[1],Rot[0]);
//    TooN::SO3<> r(R);
    //cout<<r.get_matrix();
    //    Rot=r*Point3d(0,0,-16384.0);
    //    return Point3d(Rot[0],Rot[1],Rot[2]);
    return Point3d(R[2][0]*-16384.0,R[2][1]*-16384.0,R[2][2]*-16384.0);
}

void IMU::CalibrateIMU(int sampleNum)
{
    sampleNum=svar.GetInt("CalibrateSampleNum",sampleNum);
    isCalibrating=true;
    insertFrame=false;
    while(data.size()) data.pop_front();
    cout<<"We are now starting to calibrate the IMU.Please keep the IMU still and input y to confirm,any otherkey to cancel.\n";
    char key;
    cin>>key;
    if(key=='y')
    {
        vector<Point3d > measure,shouldB;
        cout<<"Calibrating ...\n";
        while(sampleNum--)
        {
            cout<<"Sample rested:"<<sampleNum<<",change the pose of IMU and enter y to continue.\n";
            cin>>key;
            insertFrame=true;
            while(data.size()<=svar.GetInt("CalibrateSampleEach",200)) sleep(2);
            insertFrame=false;
            Point3d sampleM(Point3d(0,0,0)),sampleB(Point3d(0,0,0));
            int num=data.size();
            while(data.size()>0)
            {
                AHRS_Frame FF=data.front().frame;
                data.pop_front();
                sampleM[0]+=FF.Ax;
                sampleM[1]+=FF.Ay;
                sampleM[2]+=FF.Az;
                sampleB[2]+=FF.yaw;
                sampleB[1]+=FF.pitch;
                sampleB[0]+=FF.roll;
            }
            sampleM=sampleM/num;
            sampleB=sampleB/num;
            cout<<"sample:Rot:"<<sampleB<<endl;
            sampleB=earth_g2IMU(sampleB);
            msg(",Measure="<<sampleM<<",ShouldB="<<sampleB<<",enter y to add this sample."<<endl);
            //            cin>>key;
            key='y';
            if(key=='y')
            {
                measure.push_back(sampleM);
                shouldB.push_back(sampleB);
            }
            else
            {
                sampleNum++;
            }
        }
        drift.CalibrateFromeVector(measure,shouldB);
        drift.SaveCalibration2File("drift.txt");

    }
}

pi::SE3<> IMU::ComputeFrame(IMU_Frame &lastFrame,IMU_Frame &frame2Compute)
{

    msg("-------------------------------------------------------------------------\n");
    msg("New ComputeFrame:\n");
    //    lastFrame.frame.print();
    //    CurrentFrame.frame.print();
    //    msg("CurrentFrame.UpdateRotation:");
    //    CurrentFrame.UpdateRotation();
    double deta_T=(frame2Compute.frame.tm-lastFrame.frame.tm)/1000.0;  //unit: s

    if(deta_T<0||deta_T>1)
    {
        cout<<"deta_T="<<deta_T<<" LastFrameTime:"<<lastFrame.frame.tm<<" CurrentFrameTime:"<<frame2Compute.frame.tm<<endl;
        frame2Compute=lastFrame;
        return frame2Compute.pose;
    }
    msg("deta_T="<<deta_T);
    msg("CurrentFrame.AbsoluteAccelarate:");
    Point3d acc=frame2Compute.AbsoluteAccelarate();
    frame2Compute.v=lastFrame.v+acc*deta_T;

    if(int filt=svar.GetInt("UseFilter",1))
    {
        static ofstream filt_file("filter_data.txt");
        if(filt>1) filt_file<<frame2Compute.v;
        frame2Compute.filted_v=filter.getVal(frame2Compute.v);
        if(filt>1) filt_file<<frame2Compute.filted_v<<endl;


            if(svar.GetInt("UsePoseFilter",1))
            {
                frame2Compute.unfilt_trans=lastFrame.unfilt_trans+(lastFrame.filted_v+frame2Compute.filted_v)*0.5*deta_T;
                frame2Compute.pose.get_translation()=filterpose.getVal(frame2Compute.unfilt_trans);
            }
            else
            frame2Compute.pose.get_translation()=lastFrame.pose.get_translation()+(lastFrame.filted_v+frame2Compute.filted_v)*0.5*deta_T;

    }
    else
        frame2Compute.pose.get_translation()=lastFrame.pose.get_translation()+(lastFrame.v+frame2Compute.v)*0.5*deta_T;
    //cout<<"ComputeInfo:\ntime:"<<CurrentFrame.frame.tm<<" v:"<<CurrentFrame.v<<" pose:"<<CurrentFrame.pose.get_translation()<<endl;
    if(svar.GetInt("SaveProcess2File",0))
        frame2Compute.SaveProcess2File();

    return frame2Compute.pose;
}

pi::SE3<> IMU::ComputeFrame(IMU_Frame &lastFrame)
{
    ComputeFrame(lastFrame,CurrentFrame);
}

bool IMU_Frame::isValid()
{
    if(frame.Ax<-65536||frame.Ax>65536||frame.Ay<-65536||frame.Ay>65536||frame.Az<-65536||frame.Az>65536) return false;
    if(frame.Gx<-65536||frame.Gx>65536||frame.Gy<-65536||frame.Gy>65536||frame.Gz<-65536||frame.Gz>65536) return false;
    if(frame.yaw>360||frame.yaw<-360||frame.pitch>360||frame.pitch<-360||frame.roll>360||frame.roll<-360) return false;
    return true;
}
void IMU_Frame::SaveProcess2File()
{
    static ofstream file("acc.txt");
    file<<acc<<endl;
}

void IMU::setPose(pi::SE3<> pose,Point3d v,u_int64 time)
{
    filter.reset();
    filterpose.reset();
    while(data.front().frame.tm<time-5)
        data.pop_front();
    data.front().pose=pose;
    data.front().v=v;
}

pi::SE3<> IMU::ComputePose(u_int64 time)
{
//    filter.reset();
//    filterpose.reset();
    bool doCompute=svar.GetInt("ComputeFrame",0);
    int i,n=data.size();
    for(i=0;i<n;i++)
    {
        IMU_Frame &lastFrame=data[i];
        IMU_Frame &currentFrame=data[i+1];
        if(!currentFrame.isValid())
        {
            currentFrame.frame.print();
            currentFrame=lastFrame;
            continue;
        }
        currentFrame.UpdateRotation();
        if(doCompute) ComputeFrame(lastFrame,currentFrame);
        CurrentFrame=currentFrame;
        if(currentFrame.frame.tm>=time) break;
    }

    //Ensure that the deque only save less than 1s computed data
    int data2SaveNum=svar.GetInt("Data2SaveNum", 2);
    for(;i>data2SaveNum;i--)
    {
        data.pop_front();
    }
    return CurrentFrame.pose;
}

void IMU::run()
{
    while(1)
    {
        if( ahrs.frame_ready()&&insertFrame) {
            AHRS_Frame frame,frame2put;
            frame = ahrs.frame_get();
            frame2put = ahrs.frame_get(AHRS_Frame::FRAME_SENSOR);
            //change the data to right hand axis
            frame2put.roll=frame.roll;
            frame2put.pitch=-frame.pitch;
            frame2put.yaw=-frame.yaw;
            frame2put.alt=frame.alt;
            frame2put.Ax=-frame2put.Ax;
            frame2put.Ay=-frame2put.Ay;
            frame2put.Az=-frame2put.Az;
            if(!isCalibrating)
            {
                frame2put.Ax=drift.getAx(frame2put.Ax);
                frame2put.Ay=drift.getAy(frame2put.Ay);
                frame2put.Az=drift.getAz(frame2put.Az);
            }
            if(1)
            {
//                static ofstream Spill_file("spill.txt");
//                Spill_file<<frame2put.Ax<<" "<<frame2put.Ay<<" "<<frame2put.Az<<" ";
                spill_handle.GetTrueVal(frame2put.Ax,frame2put.Ay,frame2put.Az);
//                Spill_file<<frame2put.Ax<<" "<<frame2put.Ay<<" "<<frame2put.Az<<"\n";
            }
            IMU_Frame FF(frame2put);
            data.push_back(FF);
            //            frame2put.print();
            double acc=sqrt(frame2put.Ax*frame2put.Ax+frame2put.Ay*frame2put.Ay+frame2put.Az*frame2put.Az);
//                        cout<<"insert "<<FF.frame.tm<<",acc="<<acc<<endl;
        }
        else
            sleep(2);
    }
}

TooN::SO3<> rebuildSO3(double yaw,double pitch,double roll)
{
    TooN::Matrix<3,3,double> Rot=rebuildRotation(yaw,pitch,roll);
    return TooN::SO3<>(Rot);
}

template <class ValType>
TooN::Matrix<3,3,double> rebuildRotation(const ValType &m_yaw,const ValType &m_pitch,const ValType &m_roll)
{
    //right hand axis z=cross(x,y)        ZYX
    const double	cy = cos(m_yaw*deg2rad);
    const double	sy = sin(m_yaw*deg2rad);
    const double	cp = cos(m_pitch*deg2rad);
    const double	sp = sin(m_pitch*deg2rad);
    const double	cr = cos(m_roll*deg2rad);
    const double	sr = sin(m_roll*deg2rad);
    TooN::Matrix<3,3,double> m(TooN::Data(
                             cy*cp,      cy*sp*sr-sy*cr,     cy*sp*cr+sy*sr,
                             sy*cp,      sy*sp*sr+cy*cr,     sy*sp*cr-cy*sr,
                             -sp,        cp*sr,              cp*cr          ));
    return m;
}

}
#endif

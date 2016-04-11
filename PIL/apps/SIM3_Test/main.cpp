#include <iostream>

#include "base/Svar/Svar_Inc.h"
#include "base/time/Global_Timer.h"
#include "base/types/SE3.h"

#include  <TooN/se3.h>


#include  <sophus/se3.h>

using namespace pi;
using namespace std;

Sophus::SE3 toSophus(pi::SE3<double> se3_zy)
{
    Eigen::Matrix3d eigen_rot;
    se3_zy.get_rotation().inv().getMatrixUnsafe(eigen_rot);
    Eigen::Vector3d eigen_trans=*((Eigen::Vector3d*)&se3_zy.get_translation());

    return Sophus::SE3(eigen_rot,eigen_trans);
}


bool TestMatrix()
{
    pi::SE3<double> se3_zy(1.0,2.0,3.0,
                           0.150907,0.301814,0.452721,-0.825336);
    cout<<"se3_zy="<<se3_zy<<endl;

    TooN::Matrix<3,4,double> matrix;
    se3_zy.getMatrixUnsafe(matrix);
    cout<<"getmatrix=\n"<<matrix;

    se3_zy.fromMatrixUnsafe(matrix);
    cout<<"frommatrix="<<se3_zy<<endl;

    se3_zy.getMatrixUnsafe(matrix);
    cout<<"getmatrix=\n"<<matrix;

    pi::Point3f p3f=se3_zy.get_translation();
    cout<<"p3f="<<p3f<<endl;

    pi::SE3<float> se3_f(se3_zy.get_rotation(),se3_zy.get_translation());
    cout<<"se3_float="<<se3_f<<endl;

    pi::SE3<>   se3_d=se3_f;
    cout<<"se3_double="<<se3_d<<endl;

    cout<<"pi::ln()="<<se3_d.get_rotation().ln()<<endl;
    cout<<"pi::exp()="<<pi::SO3<>::exp(se3_d.get_rotation().ln())<<endl;

    TooN::SE3<> toon_se3=se3_d;
    cout<<"TooN::ln()="<<toon_se3.get_rotation().ln()<<endl;

    Sophus::SE3 sophus_se3=toSophus(se3_zy);
    cout<<"SE3_Sophus:"<<sophus_se3<<endl
       <<sophus_se3.matrix()<<endl;
}

bool TestMultiply()
{
    cout<<"###########################################################\n";
    cout<<"Testing multiply...\n";
    pi::SE3<double> s1(1.0,2.0,3.0,
                           0.150907,0.301814,0.452721,-0.825336);
    pi::Point3_<double> p(5,6,7);
    pi::SE3<double> s2=s1*s1;
    TooN::Matrix<3,4,double> m;
    s2.getMatrixUnsafe(m);
    cout<<"Result of ZY:\n"<<m;

    TooN::SE3<double> t1=s1;
    cout<<"Result of TooN:\n"<<t1*t1;
    cout<<"###########################################################\n";

    cout<<"###########################################################\n";
    cout<<"Point transform:\n";
    cout<<"TooN:"<<t1*(*(TooN::Vector<3,double>*)&p)<<endl;
    pi::Point3_<double> result=s1*p;
    cout<<"ZY:"<<result<<endl;
    cout<<"###########################################################\n";
}

template <typename Precision>
bool TimeUseage()
{
    int times=100000;
    pi::SO3<Precision> s1,s2;
    s1.FromAxis(pi::Point3_<Precision>(1.0,2.0,3.0),2);
    s2.FromAxis(pi::Point3_<Precision>(2.0,3.0,4.0),2);

    pi::SE3<Precision> e1(s1,pi::Point3_<Precision>(4,5,6));
    pi::SE3<Precision> e2(s2,pi::Point3_<Precision>(4,5,6));

    TooN::SE3<Precision> t1,t2;
    t1=e1;
    t2=e2;


    Sophus::SE3 sp1=toSophus(e1);
    Sophus::SE3 sp2=toSophus(e2);

    TooN::Vector<3,Precision> p=TooN::makeVector(1.5,2.5,3.5);
    pi::Point3_<Precision>& pz=*((pi::Point3_<Precision>*)&p);

    if(sizeof(Precision)==4)//float
    {
        cout<<"###########################################################\n";
        cout<<"TimeUsage testing...\n";
        cout<<"Transform1:\n"<<t1
           <<"Transform2:\n"<<t2
          <<"Point:"<<pz<<endl;
        //Transform pose
        timer.enter("Float::PoseTooN");
        for(int i=0;i<times;i++)
            t1*t2;
        timer.leave("Float::PoseTooN");
        cout<<"Float::PoseTooN:\n"<<t1*t2;

        timer.enter("Float::PoseZY");
        for(int i=0;i<times;i++)
            e1*e2;
        timer.leave("Float::PoseZY");
        cout<<"Float::PoseZY:\n"<<(TooN::SE3<Precision>)(e1*e2);

        //Transform points
        timer.enter("Float::PointTooN");
        for(int i=0;i<times;i++)
            t1*p;
        timer.leave("Float::PointTooN");
        cout<<"Float::PointTooN:"<<t1*p<<endl;

        timer.enter("Float::PointZY");
        for(int i=0;i<times;i++)
            e1*pz;
        timer.leave("Float::PointZY");
        cout<<"Float::PointZY:"<<e1*pz<<endl;
    }
    else
    {
        //Transform pose
        timer.enter("Double::PoseTooN");
        for(int i=0;i<times;i++)
            t1*t2;
        timer.leave("Double::PoseTooN");

        timer.enter("Double::PoseZY");
        for(int i=0;i<times;i++)
            e1*e2;
        timer.leave("Double::PoseZY");
        cout<<"Double::PoseZY:\n"<<e1*e2<<endl;

        timer.enter("Double::PoseSophus");
        for(int i=0;i<times;i++)
            sp1*sp2;
        timer.leave("Double::PoseSophus");
        Sophus::SE3 result_=sp1*sp2;
        cout<<"Sophus1:"<<sp1<<endl;
        cout<<"Sophus2:"<<sp2<<endl;
        cout<<"Double::PoseSophus:\n"<<result_.matrix()<<endl;

        //Transform points
        Eigen::Vector3d &eigen_p=*((Eigen::Vector3d*)&p);
        timer.enter("Double::PointTooN");
        for(int i=0;i<times;i++)
            t1*p;
        timer.leave("Double::PointTooN");

        timer.enter("Double::PointZY");
        for(int i=0;i<times;i++)
            e1*pz;
        timer.leave("Double::PointZY");
        cout<<"Double::PointZY:"<<(e1*pz)<<endl;

        timer.enter("Double::PointSophus");
        for(int i=0;i<times;i++)
            sp1*eigen_p;
        timer.leave("Double::PointSophus");
        cout<<"Double::PointSophus:"<<(sp1*eigen_p)<<endl;
    }
    cout<<"###########################################################\n";
}

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);
    TestMatrix();
    TestMultiply();
    TimeUseage<float>();
    TimeUseage<double>();
    return 0;
}


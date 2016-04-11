#include <iostream>

#include "base/Svar/Svar_Inc.h"
#include "base/time/Global_Timer.h"
#include "base/types/ScSO3.h"

#include "TooN/se3.h"

#include "base/types/SE3.h"
#include  <sophus/scso3.h>
#include  <sophus/se3.h>

using namespace pi;
using namespace std;

Sophus::ScSO3 toSophus(pi::ScSO3<double> se3_zy)
{
    Eigen::Matrix3d eigen_rot;
    se3_zy.inv().getMatrixUnsafe(eigen_rot);
    return Sophus::ScSO3(eigen_rot);
}

Sophus::SE3 toSophus(pi::SE3<double> se3_zy)
{
    Eigen::Matrix3d eigen_rot;
    se3_zy.get_rotation().inv().getMatrixUnsafe(eigen_rot);
    Eigen::Vector3d eigen_trans=*((Eigen::Vector3d*)&se3_zy.get_translation());

    return Sophus::SE3(eigen_rot,eigen_trans);
}

bool TestMatrix()
{
    pi::ScSO3<double> zy(1.0,2.0,3.0,4.0);
    cout<<"zy="<<zy<<endl;

    TooN::Matrix<3,3,double> matrix;
    zy.getMatrixUnsafe(matrix);
    cout<<"getmatrix=\n"<<matrix;

    zy.fromMatrixUnsafe(matrix);
    cout<<"frommatrix="<<zy<<endl;

    zy.getMatrixUnsafe(matrix);
    cout<<"getmatrix=\n"<<matrix;

    pi::ScSO3<float> se3_f(zy);
    cout<<"se3_float="<<se3_f<<endl;

    pi::ScSO3<double> se3_d=se3_f;
    cout<<"se3_double="<<se3_d<<endl;

    cout<<"pi::ln()="<<se3_d.ln()<<endl;
    cout<<"pi::exp()="<<pi::ScSO3<double>::exp(se3_d.ln())<<endl;

    Sophus::ScSO3 sophus_se3=toSophus(zy);
    cout<<"SE3_Sophus:"<<sophus_se3.log()<<endl
       <<sophus_se3.matrix()<<endl;
}

bool TestMultiply()
{
    cout<<"###########################################################\n";
    cout<<"Testing multiply...\n";
    pi::ScSO3<double> s1(1.0,2.0,3.0,4.0);
    pi::Point3_<double> p(5,6,7);
    pi::ScSO3<double> s2=s1*s1;
    TooN::Matrix<3,3,double> m;
    s2.getMatrixUnsafe(m);
    cout<<"Result of ZY:\n"<<m<<s1*s1<<endl;

    Sophus::ScSO3 t1=toSophus(s1);
    cout<<"Result of Sophus:\n"<<(t1*t1).matrix()<<endl<<t1*t1<<endl;
    cout<<"###########################################################\n";

    cout<<"###########################################################\n";
    cout<<"Point transform:\n";
    cout<<"Sophus:"<<t1*(*(Eigen::Vector3d*)&p)<<endl;
    pi::Point3_<double> result=s1*p;
    cout<<"ZY:"<<result<<endl;
    cout<<"###########################################################\n";
}

template <typename Precision>
bool TimeUseage()
{
    int times=100000;
    pi::ScSO3<Precision> e1(1.0,2.0,3.0,2),e2(2.0,3.0,4.0,2);

    Sophus::ScSO3 sp1=toSophus(e1);
    Sophus::ScSO3 sp2=toSophus(e2);

    pi::Point3_<Precision> pz(5,6,7);

    //Transform pose
    timer.enter("Double::PoseZY");
    for(int i=0;i<times;i++)
        e1*e2;
    timer.leave("Double::PoseZY");
    cout<<"Double::PoseZY:\n"<<e1*e2<<endl;

    timer.enter("Double::PoseSophus");
    for(int i=0;i<times;i++)
        sp1*sp2;
    timer.leave("Double::PoseSophus");
    Sophus::ScSO3 result_=sp1*sp2;
    cout<<"Sophus1:"<<sp1<<endl;
    cout<<"Sophus2:"<<sp2<<endl;
    cout<<"Double::PoseSophus:\n"<<result_<<endl;

    //Transform points
    Eigen::Vector3d &eigen_p=*((Eigen::Vector3d*)&pz);
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
    cout<<"###########################################################\n";
}

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);
    TestMatrix();
    TestMultiply();
    TimeUseage<double>();
    return 0;
}


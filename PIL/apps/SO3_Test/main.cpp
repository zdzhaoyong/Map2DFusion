#include <iostream>

#include "base/Svar/Svar_Inc.h"
#include "base/time/Global_Timer.h"
#include "base/types/SO3.h"
#include <base/utils/utils_str.h>

#include  <TooN/so3.h>

using namespace pi;
using namespace std;
template<typename Precision>
bool TestMatrix()
{
    pi::SO3<Precision> so3_zy;
    so3_zy.FromAxis(pi::Point3_<Precision>(1.0,2.0,3.0),1.2);
    cout<<"ZY:\n"<<so3_zy<<endl;

    TooN::SO3<Precision> so3_toon;
    so3_zy.getMatrixUnsafe(so3_toon.get_matrix());
    cout<<"TooN:\n"<<so3_toon;

    TooN::Matrix<3,3,Precision> m=so3_toon.get_matrix();
    so3_zy.fromMatrixUnsafe(m);
    cout<<"ZY:\n"<<so3_zy<<endl;

    pi::SO3f so3_f=so3_zy;
    cout<<"so3_f="<<so3_f<<endl;
}

template<typename Precision>
bool TestMultiply()
{
    cout<<"Testing multiply...\n";
    pi::SO3<Precision> s1;
    s1.FromAxis(pi::Point3_<Precision>(1.0,2.0,3.0),2);
    pi::Point3_<Precision> p(5,6,7);
    pi::SO3<Precision> s2=s1*s1;
    TooN::Matrix<3,3,Precision> m;
    s2.getMatrixUnsafe(m);
    cout<<"Result of ZY:\n"<<m;

    s1.getMatrixUnsafe(m);
    cout<<"Result of Matrix:\n"<<m*m;

    cout<<"Point transform:\n";
    cout<<m*(*(TooN::Vector<3,Precision>*)&p)<<endl;
    pi::Point3_<Precision> result=s1*p;
    cout<<result<<endl;
}

bool TestEuler()
{
    cout<<"Testing Euler...\n";
    pi::SO3f s1;
    s1.FromEuler(0.1,0.2,0.3);
    cout<<"SO3:"<<s1<<endl;
    cout<<"\nPitch:"<<s1.getPitch()<<"\nYaw:"<<s1.getYaw()<<"\nRoll:"<<s1.getRoll()<<endl;
}

template<typename Precision>
bool TimeUseage()
{
    int times=1000000;
    pi::SO3<Precision> s1,s2;
    vector<pi::SO3<Precision> > resultS(times);
    s1.FromAxis(pi::Point3_<Precision>(2.0,2.0,3.0),2);
    s2.FromAxis(pi::Point3_<Precision>(2.0,3.0,4.0),2);

    TooN::SO3<Precision> t1,t2,resultT;
    s1.getMatrixUnsafe(t1);
    s2.getMatrixUnsafe(t2);

    TooN::Vector<3,Precision> p=TooN::makeVector(1,2,3),Tresult;
    pi::Point3_<Precision>& pz=*((pi::Point3_<Precision>*)&p),Presult;

    cout<<t1<<t2<<pz<<endl;
    //Transform pose
    timer.enter((pi::itos(sizeof(Precision))+"PoseTooN").c_str());
    for(int i=0;i<times;i++)
        resultT=t1*t2;
    timer.leave((pi::itos(sizeof(Precision))+"PoseTooN").c_str());
    cout<<"resultT:"<<resultT;

    timer.enter((pi::itos(sizeof(Precision))+"PoseZY").c_str());
    for(int i=0;i<times;i++)
    {
        resultS[i]=s1*s2;
    }
    timer.leave((pi::itos(sizeof(Precision))+"PoseZY").c_str());
    cout<<",resultS[0]:"<<resultS[0];

    //Transform points
    timer.enter((pi::itos(sizeof(Precision))+"PointTooN").c_str());
    for(int i=0;i<times;i++)
        Tresult=t1*p;
    timer.leave((pi::itos(sizeof(Precision))+"PointTooN").c_str());
    cout<<",Tresult:"<<Tresult;

    timer.enter((pi::itos(sizeof(Precision))+"PointZY").c_str());
    for(int i=0;i<times;i++)
        Presult=s1*pz;
    timer.leave((pi::itos(sizeof(Precision))+"PointZY").c_str());
    cout<<",Presult:"<<Presult<<endl;
}

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);
    TestMatrix<double>();
    TestMultiply<double>();
    TestEuler();
    TimeUseage<float>();
    TimeUseage<double>();
    return 0;
}

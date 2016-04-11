#include <iostream>

#include <base/Svar/Svar_Inc.h>
#include <base/time/Global_Timer.h>
#include <base/Svar/Scommand.h>
#include <base/Svar/VecParament.h>
#include <base/types/types.h>

using namespace std;
using namespace pi;

void builtin_test(void* /* ptr */, string /* sCommand */, string sParams)
{

    timer.leave("cmd.Call");
    //test write
    cout<<"Parament(Sucess)="<<sParams<<endl;

    timer.enter("Svar.TestInt.First");
    svar.GetInt("TestInt",1000);
    timer.leave("Svar.TestInt.First");

    timer.enter("Svar.TestDouble.First");
    svar.GetDouble("TestInt",1000);
    timer.leave("Svar.TestDouble.First");

    timer.enter("Svar.TestString.First");
    svar.GetString("TestInt","1000");
    timer.leave("Svar.TestString.First");


    //test read
    for(int i=0;i<1000;i++)
    {
        timer.enter("Svar.TestInt");
        svar.GetInt("TestInt",1000);
        timer.leave("Svar.TestInt");


        timer.enter("Svar.TestDouble");
        svar.GetDouble("TestInt",1000);
        timer.leave("Svar.TestDouble");

        timer.enter("Svar.TestString");
        svar.GetString("TestInt","1000");
        timer.leave("Svar.TestString");
    }
    cout<<"SvarWithType.TestInt(1000)="<<svar.GetInt("SvarWithType.TestInt",1000)<<endl;

    //SvarWithType test
    timer.enter("SvarWithType.TestInt.First");
    int &test_int=svar.GetInt("SvarWithType.TestInt",1000);
    timer.leave("SvarWithType.TestInt.First");

    timer.enter("SvarWithType.TestDouble.First");
    double &test_double=svar.GetDouble("SvarWithType.TestDouble",1000);
    timer.leave("SvarWithType.TestDouble.First");

    timer.enter("SvarWithType.TestString.First");
    string &test_string=svar.GetString("SvarWithType.TestString","1000");
    timer.leave("SvarWithType.TestString.First");

    cout<<"SvarWithType.TestInt(1000)="<<svar.GetInt("SvarWithType.TestInt",1000)<<endl;
    cout<<"SvarWithType.TestDouble(1000)="<<svar.GetDouble("SvarWithType.TestDouble",1000)<<endl;
    cout<<"SvarWithType.TestString(1000)="<<svar.GetString("SvarWithType.TestString","1000")<<endl;

    test_int=10000;
    test_double=10000;
    test_string="10000";

    cout<<"SvarWithType.TestInt(10000)="<<svar.GetInt("SvarWithType.TestInt",1000)<<endl;
    cout<<"SvarWithType.TestDouble(10000)="<<svar.GetDouble("SvarWithType.TestDouble",1000)<<endl;
    cout<<"SvarWithType.TestString(10000)="<<svar.GetString("SvarWithType.TestString","1000")<<endl;

    svar.i["SvarWithType.TestInt"]=0;

    cout<<"SvarWithType.TestInt(0)="<<test_int<<endl;

    svar.ParseLine("Paraments ?=[0 1 2 3 4] ");
    VecParament vec5;
    vec5=svar.get_var("Paraments",vec5);
    cout<<"Paraments([0 1 2 3 4])="<<vec5.toString()<<endl;

    svar.ParseLine("Point3D = 0 1 2");
    Point3ub p3d(0,0,0);
    p3d=svar.get_var("Point3D",p3d);
    cout<<"Point3D(0 1 2)="<<p3d<<endl;

    string str_result=svar.GetString("Scommand.TestResult","Failed");
    cout<<"Scommand.TestResult(Success)="<<str_result;


    scommand.Call("system","mkdir -p testSystem");
}

int main(int argc,char **argv)
{
    timer.enter("GlobalMain");
    //test parse
    timer.enter("Svar.Parse");
    svar.ParseMain(argc,argv);
    timer.leave("Svar.Parse");

    timer.enter("cmd.RegisterCommand");
    scommand.RegisterCommand("main_test",builtin_test);
    timer.leave("cmd.RegisterCommand");

    timer.enter("cmd.Call");
    scommand.Call("main_test Success");
    timer.leave("GlobalMain");
    return 0;
}



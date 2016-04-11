#The PI_Base project 
------------------------------------------------------------------------------

##1. Introduction
------------------------------------------------------------------------------
The PI_Base is the base part of [PIL](https://github.com/zdzhaoyong/PIL) library, which includes some pretty useful tools for 
C++ programing espesially in the areas of moblile robotics and computer vision. 
Since parameters parsing and time useage statistics are two of the most needed fuction
for program development, we provide an enficient and thread safe implimentation of them,
and this documentation will focus on their usage introduction.

##2. Resources and Compilation
------------------------------------------------------------------------------
###2.1. Resources
  * Download the latest code with: 
    * Git: 
    
            git clone https://github.com/zdzhaoyong/PI_Base
  * Also you can download the whole PIL library: 
    * Git: 
    
            git clone https://github.com/zdzhaoyong/PIL

###2.2. Compilation
As we develop the whole project on Linux, only compilation *Makefile* for linux is provided. 
But since only *pthread* dependency is required (acqually no dependency for linux), 
it can be easily transplanted on Windows, Mac or other embedded systems.

If you are using linux systems, it can be compiled with one command:

    cd PI_Base;make

We highly recommand users to compile it with *c++11* standard as some fuctions in *std::tr1*
should be used.


##3. Usage and demos
------------------------------------------------------------------------------
###3.1. Run the demo
After compilation, the application will be listed in folder *bin*,
but since it does not hold the data, we highly recommand you to run the demo at the application folder.
Run the demo Svar_Test:

`cd apps/Svar_Test`

`make run`

###3.2. Svar usage introduction
To introduce the two class Svar and Timer, a simple demo is demostrated below.
It contains some of the base usages which are very easy to use and we divided them into two parts:
>1. Config file writing. This file should be parsed by the program 
and you can also run the program here.


>2. C++ codes. This loaded the config file at the right time (most in the main function).

#### 3.2.1. How to write a config file?
With the `svar.ParseMain(argc,argv)` fuction lauched, the program will firstly find the
"ProgramName.cfg" file at the current folder, otherwise the "Default.cfg" will be loaded instead.
Here shows a demo config file used in demo Svar_Test.
```js
// file Default.cfg
// This is a demo Svar config file, you can set up the parameters
// here and some simple functions is supported.

// 1.Build in paraments: argv0 ProgramPath ProgramName Svar.ParsingFile
// Svar.ParsingPath Svar.ParsingName
echo ProgramPath=$(ProgramPath) 
echo ProgramName=$(ProgramName)
echo parsing Path:$(Svar.ParsingPath) File:$(Svar.ParsingName)

// 2.Function Demo

echo Parsing functions
function loadCamera
    echo Function loadCamera called.
    echo Parsing $(CameraConfigFile).
    //Parse another file
    include $(CameraConfigFile)
endfunction

function loadTestCommand
    include data/TestCommand.cfg
endfunction

// 3.Parament settings
echo Setting Paraments
ShouldCall      =loadCamera
CameraConfigFile=Camera.cfg
// ?= won't overwrite existed value
TestLanguage    = Success
TestLanguage   ?= Error!
//reference is suported
${TestLanguage} = ${TestLanguage} (should be Success)

// 4.Judgement commands:if，else, endif，
// != are also supported
echo Running if else
if ${ShouldCall} = loadCamera
    echo $(ShouldCall) and loadCamera is equal!
    loadCamera
    loadTestCommand
else
    loadTestCommand
    echo $(ShouldCall) and loadCamera is not equal!
endif //fi is also ok

// 5.One can get the Default value from SvarWithType
GetInt SvarWithType.TestInt
echo SvarWithType.TestInt=$(SvarWithType.TestInt)
//Now SvarWith.TestInt may be 0, and program need 1000, so
SvarWithType.TestInt=1000

echo I am parsing Path:$(Svar.ParsingPath) File:$(Svar.ParsingName)

echo Finished $(Svar.ParsingFile)
```
####3.2.2. What Svar can do and how to use it?
The Svar class parse your config file and commands, 
so that parameters can be obtained and commands can be handled easily through out the whole process.
With template class SvarWithType, any class infomation can be shared with a string key,
this is pretty cool!
In the follow demo Svar_Test, the time usage statistic tool is also used, really easy!
```
// file main.cpp
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

    svar.ParseLine("Paraments =[0 1 2 3 4] ");
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
```
####3.2.3. Running result
With the above program and config file, the result could be：
```
Parsing file ./Default.cfg ....
ProgramPath=./../../bin 
ProgramName=Svar_Test
parsing Path:. File:Default.cfg
Parsing functions
Setting Paraments
Running if else
loadCamera and loadCamera is equal!
Function loadCamera called.
Parsing Camera.cfg.
!Svar::ParseFile: Failed to load script file "Camera.cfg".
I am parsing Path:data File:TestCommand.cfg
finished TestCommand.cfg
SvarWithType.TestInt=0
I am parsing Path:. File:Default.cfg
Finished ./Default.cfg
Parament(Sucess)=Success
SvarWithType.TestInt(1000)=1000
SvarWithType.TestInt(1000)=1000
SvarWithType.TestDouble(1000)=1000
SvarWithType.TestString(1000)=1000
SvarWithType.TestInt(10000)=10000
SvarWithType.TestDouble(10000)=10000
SvarWithType.TestString(10000)=10000
SvarWithType.TestInt(0)=0
Paraments([0 1 2 3 4])=[0 1 2 3 4]
Point3D(0 1 2)=0 1 2
Scommand.TestResult(Success)=Success
---------------------------- ZhaoYong::Timer report --------------------------
           FUNCTION                       #CALLS  MIN.T  MEAN.T  MAX.T  TOTAL 
------------------------------------------------------------------------------
GlobalMain                                   1    5.1ms   5.1ms   5.1ms   5.1ms
Svar.Parse                                   1  854.0us 854.0us 854.0us 854.0us
Svar.TestDouble                           1000    0.0ps 211.0ns   1.0us 211.0us
Svar.TestDouble.First                        1    3.0us   3.0us   3.0us   3.0us
Svar.TestInt                              1000    0.0ps 243.0ns   4.0us 243.0us
Svar.TestInt.First                           1    3.0us   3.0us   3.0us   3.0us
Svar.TestString                           1000    0.0ps 285.0ns   2.0us 285.0us
Svar.TestString.First                        1    3.0us   3.0us   3.0us   3.0us
SvarWithType.TestDouble.First                1    2.0us   2.0us   2.0us   2.0us
SvarWithType.TestInt.First                   1    0.0ps   0.0ps   0.0ps   0.0ps
SvarWithType.TestString.First                1    2.0us   2.0us   2.0us   2.0us
cmd.Call                                     1    6.0us   6.0us   6.0us   6.0us
cmd.RegisterCommand                          1    6.0us   6.0us   6.0us   6.0us
----------------------- End of ZhaoYong::Timer report ------------------------
```
##4. Contact
------------------------------------------------------------------------------
If you have any problem, or you have some suggestions for this code, 
please contact Yong Zhao by zd5945@126.com, thank you very much!


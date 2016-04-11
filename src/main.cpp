#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>

#include <base/Svar/Svar.h>
#include <base/Svar/VecParament.h>
#include <base/time/Global_Timer.h>
#include "MainWindow.h"

#include "Map2D.h"

using namespace std;

class TrajectoryLengthCalculator
{
public:
    TrajectoryLengthCalculator():length(-1){}
    ~TrajectoryLengthCalculator()
    {
        cout<<"TrajectoryLength:"<<length<<endl;
    }

    void feed(pi::Point3d position)
    {
        if(length<0)
        {
            length=0;
            lastPosition=position;
        }
        else
        {
            length+=(position-lastPosition).norm();
            lastPosition=position;
        }
    }

private:
    double      length;
    pi::Point3d lastPosition;
};


class TestSystem:public pi::Thread,public pi::gl::EventHandle
{
public:
    TestSystem()
    {
        if(svar.GetInt("Win3D.Enable",1))
        {
            mainwindow=SPtr<MainWindow>(new MainWindow(0));
        }

    }
    ~TestSystem()
    {
        stop();
        while(this->isRunning()) sleep(10);
        if(map.get())
            map->save(svar.GetString("Map.File2Save","result.png"));
        map=SPtr<Map2D>();
        mainwindow=SPtr<MainWindow>();
    }

    virtual bool KeyPressHandle(void* arg)
    {
        QKeyEvent* e=(QKeyEvent*)arg;
        switch (e->key()) {
        case Qt::Key_I:
        {
            std::pair<cv::Mat,pi::SE3d> frame;
            if(obtainFrame(frame))
            {
                pi::timer.enter("Map2D::feed");
                map->feed(frame.first,frame.second);
                if(mainwindow.get()&&tictac.Tac()>0.033)
                {
                    tictac.Tic();
                    mainwindow->update();
                }
                pi::timer.leave("Map2D::feed");
            }
        }
            break;
        case Qt::Key_P:
        {
            int& pause=svar.GetInt("Pause");
            pause=!pause;
        }
            break;
        case Qt::Key_Escape:
        {
            stop();
            return false;
        }
            break;
        default:
            return false;
            break;
        }
        return false;
    }

    int TestMap2DItem()
    {
        cv::Mat img=cv::imread(svar.GetString("TestMap2DItem.Image","data/test.png"));
        if(img.empty()||!mainwindow.get())
        {
            cerr<<"No image or mainwindow found.!\n";
            return -1;
        }
//        cv::imshow("img",img);
        SvarWithType<cv::Mat>::instance()["LastTexMat"]=img;

        mainwindow->getWin3D()->SetEventHandle(this);
        mainwindow->getWin3D()->setSceneRadius(1000);
        mainwindow->call("show");
        mainwindow->call("MapWidget"+svar.GetString(" TestMap2DItem.cmd",
        " Map2DUpdate LastTexMat 34.257287 108.888931 0 34.253234419307354 108.89463874078366 0"));
    }

    bool obtainFrame(std::pair<cv::Mat,pi::SE3d>& frame)
    {
        string line;
        if(!getline(*in,line)) return false;
        stringstream ifs(line);
        string imgfile;
        ifs>>imgfile;
        imgfile=datapath+"/rgb/"+imgfile+".jpg";
        pi::timer.enter("obtainFrame");
        frame.first=cv::imread(imgfile);
        pi::timer.leave("obtainFrame");
        if(frame.first.empty()) return false;
        ifs>>frame.second;
        if(svar.exist("GPS.Origin"))
        {
            if(!lengthCalculator.get()) lengthCalculator=SPtr<TrajectoryLengthCalculator>(
                        new TrajectoryLengthCalculator());
            lengthCalculator->feed(frame.second.get_translation());
        }
        return true;
    }

    int testMap2D()
    {
        cout<<"Act=TestMap2D\n";
        datapath=svar.GetString("Map2D.DataPath","");
        if(!datapath.size())
        {
            cerr<<"Map2D.DataPath is not seted!\n";
            return -1;
        }
        svar.ParseFile(datapath+"/config.cfg");
        if(!svar.exist("Plane"));
        {
//            cerr<<"Plane is not defined!\n";
//            return -2;
        }

        if(!in.get())
            in=SPtr<ifstream>(new ifstream((datapath+"/trajectory.txt").c_str()));

        if(!in->is_open())
        {
            cerr<<"Can't open file "<<(datapath+"/trajectory.txt")<<endl;
            return -3;
        }
        deque<std::pair<cv::Mat,pi::SE3d> > frames;
        for(int i=0,iend=svar.GetInt("PrepareFrameNum",10);i<iend;i++)
        {
            std::pair<cv::Mat,pi::SE3d> frame;
            if(!obtainFrame(frame)) break;
            frames.push_back(frame);
        }
        cout<<"Loaded "<<frames.size()<<" frames.\n";

        if(!frames.size()) return -4;

        map=Map2D::create(svar.GetInt("Map2D.Type",Map2D::TypeGPU),
                          svar.GetInt("Map2D.Thread",true));
        if(!map.get())
        {
            cerr<<"No map2d created!\n";
            return -5;
        }
        VecParament vecP=svar.get_var("Camera.Paraments",VecParament());
        if(vecP.size()!=6)
        {
            cerr<<"Invalid camera parameters!\n";
            return -5;
        }
        map->prepare(svar.get_var<pi::SE3d>("Plane",pi::SE3d()),
                     PinHoleParameters(vecP[0],vecP[1],vecP[2],vecP[3],vecP[4],vecP[5]),
                    frames);

        if(mainwindow.get())
        {
            mainwindow->getWin3D()->SetEventHandle(this);
            mainwindow->getWin3D()->insert(map);
            mainwindow->getWin3D()->setSceneRadius(1000);
            mainwindow->call("show");

            if(!svar.exist("GPS.Origin")) svar.i["Fuse2Google"]=0;
            else
                svar.ParseLine("SetCurrentPosition $(GPS.Origin)");
            tictac.Tic();
        }
        else
        {
            int& needStop=svar.GetInt("ShouldStop");
            while(!needStop) sleep(20);
        }

        if(svar.GetInt("AutoFeedFrames",1))
        {
            pi::Rate rate(svar.GetInt("Video.fps",100));
            while(!shouldStop())
            {
                if(map->queueSize()<2)
                {
                    std::pair<cv::Mat,pi::SE3d> frame;
                    if(!obtainFrame(frame)) break;
                    map->feed(frame.first,frame.second);
                }
                if(mainwindow.get()&&tictac.Tac()>0.033)
                {
                    tictac.Tic();
                    mainwindow->getWin3D()->update();
                }
                rate.sleep();
            }
        }
    }

    virtual void run()
    {
        string act=svar.GetString("Act","Default");
        if(act=="TestMap2DItem") TestMap2DItem();
        else if(act=="TestMap2D"||act=="Default") testMap2D();
        else cout<<"No act "<<act<<"!\n";
    }

    string        datapath;
    pi::TicTac    tictac;
    SPtr<MainWindow>  mainwindow;
    SPtr<ifstream>      in;
    SPtr<Map2D>       map;
    SPtr<TrajectoryLengthCalculator> lengthCalculator;
};

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);

    if(svar.GetInt("Win3D.Enable",0))
    {
        QApplication app(argc,argv);
        TestSystem sys;
        sys.start();
        return app.exec();
    }
    else
    {
        TestSystem sys;
        sys.run();
    }
    return 0;
}

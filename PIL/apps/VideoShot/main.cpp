#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc,char** argv)
{
    if(argc<2)
    {
        cerr<<"Usage:\n   VideoShot videoFile [path2Save(default here)] [save all]\n";
        return -1;
    }
    string videoFile=argv[1];
    string path2save=".";
    if(argc>2)
    {
        path2save=argv[2];
    }
    bool saveall=0;
    if(argc>3)
    {
        saveall=1;
    }
    cv::VideoCapture video(videoFile);
    if(!video.isOpened())
    {
        video.open(0);
        if(!video.isOpened())
            cerr<<"Can't open video!\n";
        saveall=0;
    }

    uchar key=0;
    int i=0;
    while(key!=27)
    {
        cv::Mat img;
        video>>img;
        if(img.empty()) break;


        if(img.rows>480)
        {
            cv::Mat img2show;
            cv::resize(img,img2show,cv::Size(img.cols/2,img.rows/2));
            imshow("img",img2show);
        }
        else
            imshow("img",img);
        key=cv::waitKey(0);

        if(saveall||key=='s')//S
        {
            stringstream sst;
            sst<<i++<<".jpg";
            cv::imwrite(sst.str(),img);
            cerr<<"Saved image "<<sst.str()<<endl;
        }
//        else
//            cerr<<"Key "<<key<<" pressed!\n";
    }

    return 0;
}

#ifndef PATHTABLE_H
#define PATHTABLE_H

#include <base/types/types.h>
#include <base/debug/debug_config.h>
#include <vector>

// a class for fast access to gps path, the path should be continuable
class FastPathTable
{
public:
    FastPathTable(double step)
        :min_time(-1),step_time(step),max_time(-1)
    {
        ASSERT2(step<=0,"Step must be positive!");
    }

    inline bool Add(double timestamp,const pi::Point3d& pt)
    {
//        std::cout<<"PathTable:Adding "<<timestamp<<",PT:"<<pt<<std::endl;
        if(timestamp<=min_time) return false;
        if(min_time<0)//the first pt
        {
            min_time=timestamp;
            last=std::make_pair(timestamp,pt);
            path.push_back(last);
            return true;
        }

        double next_time=path.back().first;
        if(timestamp<=next_time) return false;

        next_time+=step_time;
        for(;next_time<=timestamp;next_time+=step_time)
        {
            pi::Point3d next_pose=last.second+((next_time-last.first)/(timestamp-last.first))
                    *(pt-last.second);
            path.push_back(std::make_pair(next_time,next_pose));
        }
        last=std::make_pair(timestamp,pt);
        max_time=timestamp;
        return true;

    }

    inline bool Add(const std::pair<double,pi::Point3d>& pt)
    {
        return Add(pt.first,pt.second);
    }

    inline bool Get(const double& timestamp,pi::Point3d& pt)
    {
        if(timestamp<min_time||timestamp>max_time)
            return false;
        //compute index
        int idx=(timestamp-min_time)/step_time;
        std::pair<double,pi::Point3d> left,right;
        if(idx+1<path.size())//between idx--idx+1
        {
            left=path[idx];
            right=path[idx+1];
        }
        else //between idx--last
        {
            left=path[idx];
            right=last;
        }
        pt=left.second+((timestamp-left.first)/(right.first-left.first))
                *(right.second-left.second);
        return true;
    }

    std::vector<std::pair<double,pi::Point3d> > path;
    std::pair<double,pi::Point3d> last;
    double min_time,step_time,max_time;
};
#endif // PATHTABLE_H

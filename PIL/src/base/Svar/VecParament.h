#ifndef VECPARAMENT_H
#define VECPARAMENT_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

class VecParament : public std::vector<double>
{
public:
    VecParament(std::string str="")
    {
        fromString(str);
    }

    bool fromString(std::string str)
    {
        std::string::size_type start,stop;
        start=str.find('[');
        stop =str.find(']');
        if(start==std::string::npos||stop==std::string::npos||start>=stop)
            return false;
        str=str.substr(start+1,stop-1);
        bool hasDot=(str.find(',')!=std::string::npos);
        char alige;
        if(hasDot)
        alige=',';
        else
        {
            alige=' ';
        }
        std::string str_num;
        while (str.size())
        {
            std::string::size_type n=str.find(alige);
            if(n!=std::string::npos)
            {
                str_num=str.substr(0,n);
                str=str.substr(n+1);
            }
            else
            {
                str_num=str;
                str="";
            }
            if(str_num==""||str_num==" ") continue;

            std::istringstream iss(str_num);
            double x;
            iss>>x;
            push_back(x);
        }

    }

    std::string toString()
    {
        std::ostringstream ost;
        ost<<"[";
        for(int i=0;i<size()-1;i++)
            ost<<at(i)<<" ";
        ost<<at(size()-1)<<"]";
        return ost.str();
    }

    friend inline std::istream& operator >>(std::istream& is,VecParament & p)
    {
        char str[256];
        is.get(str,150,'/');
        p.fromString(str);
        return is;
    }

    friend inline std::ostream& operator <<(std::ostream& os,VecParament & p)
    {
        os<<p.toString();
        return os;
    }
};

#endif // VECPARAMENT_H

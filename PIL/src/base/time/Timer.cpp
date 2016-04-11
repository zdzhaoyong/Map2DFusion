
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "../Svar/Svar_Inc.h"
#include "Time.h"
#include "Timer.h"


namespace pi {

using namespace std;

Timer timer;

#ifdef MUTI_THREAD
Mutex mutex;
#endif


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void TicTac::Tic()
{
    tmBegin = tm_getTimeStamp();
}

double TicTac::Tac()
{
    double tmEnd = tm_getTimeStamp();
    return tmEnd - tmBegin;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Rate::Rate(double frequency)
{
    cycle = 1.0/frequency;
    last_start = tm_getTimeStamp();
}

bool Rate::sleep()
{
    double tNow = tm_getTimeStamp();

    double slp = cycle - (tNow - last_start);
    if( slp > 0 ) tm_sleep(slp*1000);

    last_start = tNow;

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Timer::TCallData::TCallData() :
    n_calls	(0),
    min_t	(0),
    max_t	(0),
    mean_t	(0),
    has_time_units(true)
{

}

Timer::~Timer()
{
    if(SvarWithType<int>::instance().get_var("Timer.DumpAllStats",1))
    {
        dumpAllStats();
        SvarWithType<int>::instance()["Timer.DumpAllStats"]=0;
    }
}


void Timer::do_enter(const char *func_name)
{
    const string  s = func_name;

    map<string,TCallData>::iterator it=m_data.find(s);
    if(it==m_data.end())
    {
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mutex);
#endif
        it=m_data.insert(make_pair(s,TCallData())).first;
    }
    TCallData &d = it->second;

    d.n_calls++;
    d.open_calls.push(0);  // Dummy value, it'll be written below
    d.open_calls.top() = Tac(); // to avoid possible delays.
}

double Timer::do_leave(const char *func_name)
{
    const double tim = Tac();

    const string  s = func_name;
    TCallData &d = m_data[s];

    if (!d.open_calls.empty())
    {
        const double At = tim - d.open_calls.top();
        d.open_calls.pop();

        d.mean_t+=At;
        if (d.n_calls==1)
        {
            d.min_t= At;
            d.max_t= At;
        }
        else
        {
            if (d.min_t>At) d.min_t = At;
            if (d.max_t<At) d.max_t = At;
        }
        return At;
    }
    else return 0; // This shouldn't happen!
}

double Timer::getMeanTime(const std::string &name)  const
{
    map<string,TCallData>::const_iterator it = m_data.find(name);
    if (it==m_data.end())
         return 0;
    else return it->second.n_calls ? it->second.mean_t/it->second.n_calls : 0;
}

std::string unitsFormat(const double val,int nDecimalDigits, bool middle_space)
{
    char	prefix;
    double	mult;

    if (val>=1e12)
        {mult=1e-12; prefix='T';}
    else if (val>=1e9)
        {mult=1e-9; prefix='G';}
    else if (val>=1e6)
        {mult=1e-6; prefix='M';}
    else if (val>=1e3)
        {mult=1e-3; prefix='K';}
    else if (val>=1)
        {mult=1; prefix=' ';}
    else if (val>=1e-3)
        {mult=1e+3; prefix='m';}
    else if (val>=1e-6)
        {mult=1e+6; prefix='u';}
    else if (val>=1e-9)
        {mult=1e+9; prefix='n';}
    else if (val>=1e-12)
        {mult=1e+12; prefix='p';}
    else
        {mult=0; prefix='p';}

    ostringstream ost;
    ost<<setw(5) <<setiosflags(ios::fixed) <<setiosflags(ios::right)
      << setprecision(1)<<(val*mult);

    return ost.str()+char(prefix);
}

std::string rightPad(const std::string &str, const size_t total_len, bool truncate_if_larger)
{
    std::string r = str;
    if (r.size()<total_len || truncate_if_larger)
        r.resize(total_len,' ');
    return r;
}

std::string  aux_format_string_multilines(const std::string &s, const size_t len)
{
    std::string ret;

    for (size_t p=0;p<s.size();p+=len)
    {
        ret+=rightPad(s.c_str()+p,len,true);
        if (p+len<s.size())
            ret+="\n";
    }
    return ret;
}

std::string Timer::getStatsAsText(const size_t column_width)  const
{
    ostringstream ost;
    ost<<"------------------------------------  Timer report ------------------------------------\n";
    ost<<"           FUNCTION                       #CALLS  MIN.T  MEAN.T  MAX.T  TOTAL \n";
    ost<<"---------------------------------------------------------------------------------------\n";
    for (map<string,TCallData>::const_iterator i=m_data.begin();i!=m_data.end();++i)
    {
        const string sMinT   = unitsFormat(i->second.min_t,1,false);
        const string sMaxT   = unitsFormat(i->second.max_t,1,false);
        const string sTotalT = unitsFormat(i->second.mean_t,1,false);
        const string sMeanT  = unitsFormat(i->second.n_calls ? i->second.mean_t/i->second.n_calls : 0,1,false);

        ost << aux_format_string_multilines(i->first,39)
            << " " << setw(6) << setiosflags(ios::right) << i->second.n_calls <<"  "
            << sMinT << "s " << sMeanT << "s " << sMaxT << "s "
            << sTotalT << "s\n";
    }

    ost<<"--------------------------------- End of Timer report ---------------------------------\n";

    return ost.str();
}

void Timer::dumpAllStats(const size_t  column_width) const
{
    if(!m_data.size()) return;
    string s = getStatsAsText(column_width);
    cout<<endl<<s<<endl;
}

} // end of namespace pi

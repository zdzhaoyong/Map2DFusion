#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <vector>
#include <stack>
#include <map>
#include <string>

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class TicTac
{
public:
    TicTac(){}
    void   Tic();
    double Tac();

protected:
    double      tmBegin;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Rate
{
public:
    Rate(double frequency=1.0);
    bool sleep();

    double cycle, last_start;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Timer : public TicTac
{
private:
    bool		m_enabled;

    //! Data of all the calls:
    struct TCallData
    {
        TCallData();

        size_t n_calls;
        double min_t,max_t,mean_t;
        std::stack<double,std::vector<double> >   open_calls;
        bool has_time_units;
    };

    std::map<std::string,TCallData>  m_data;

    void do_enter( const char *func_name );
    double do_leave( const char *func_name );

public:
    Timer(bool enabled=true):m_enabled(enabled){Tic();}
    ~Timer();

    void enable(bool enabled = true) { m_enabled = enabled; }
    void disable() { m_enabled = false; }

    /** Start of a named section \sa enter */
    inline void enter( const char *func_name ) {
        if (m_enabled)
            do_enter(func_name);
    }

    /** End of a named section \return The ellapsed time, in seconds or 0 if disabled. \sa enter */
    inline double leave( const char *func_name ) {
        return m_enabled ? do_leave(func_name) : 0;
    }

    /** Return the mean execution time of the given "section", or 0 if it hasn't ever been called "enter" with that section name */
    double getMeanTime(const std::string &name) const;
    std::string getStatsAsText(const size_t column_width=80) const; //!< Dump all stats to a multi-line text string. \sa dumpAllStats, saveToCVSFile
    void dumpAllStats(const size_t column_width=80) const; //!< Dump all stats through the CDebugOutputCapable interface. \sa getStatsAsText, saveToCVSFile

};

} // end of namespace pi

#endif // TIMER_H

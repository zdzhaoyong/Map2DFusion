#ifndef SVAR_H
#define SVAR_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

#define MUTI_THREAD

#ifdef MUTI_THREAD
#include "../system/thread/ThreadBase.h"
#endif

class Svar;

//extern Svar svar;
#define svar Svar::instance()
#define GV2 svar

/** The class Svar will be shared in the same process, it help users to transform paraments use a name id,
 all paraments with a same name will got the same data. One can change it in all threads from assigned var,
 files and stream.
 */

template <typename Var_Type=void*>
class SvarWithType
{
    friend class Svar;
public:
    typedef std::map<std::string,Var_Type>      DataMap;
    typedef typename DataMap::iterator          DataIter;
    typedef std::pair<DataIter,bool>            InsertRet;

public:
    SvarWithType(){}

    /** This gives us singletons instance. \sa enter */
    static SvarWithType& instance()
    {
        static SvarWithType* inst=0;

        if(!inst)
        {
            inst = new SvarWithType();
        }

        return *inst;
    }

    inline bool exist(const std::string &name)
    {
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
        return data.find(name)!=data.end();
    }

    inline bool erase(const std::string &name)
    {
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
        data.erase(name);
        return true;
    }

    /** This insert a named var to the map,you can overwrite or not if the var has exist. \sa enter
    */
    inline bool insert(const std::string& name,const Var_Type& var,bool overwrite=false)
    {

#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
        DataIter it;
        it=data.find(name);
        if(it==data.end())
        {
            data.insert(std::pair<std::string,Var_Type>(name,var));
            return true;
        }
        else
        {
            if(overwrite) it->second=var;
            return false;
        }
    }

    /** function get_ptr() returns the pointer of the map or the var pointer when name is supplied,
     * when the var didn't exist,it will return NULL or insert the default var and return the var pointer in the map
     */
    inline DataMap* get_ptr()
    {
        return &data;
    }

    inline const DataMap& get_data()
    {
        return data;
    }

    inline Var_Type* get_ptr(const std::string &name)
    {
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
        DataIter it;
        it=data.find(name);
        if(it==data.end())
        {
            return NULL;
        }
        else
        {
            return &(it->second);
        }
    }

    inline Var_Type* get_ptr(const std::string& name,const Var_Type& def)
    {
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
        DataIter it;
        it=data.find(name);
        if(it==data.end())
        {
            InsertRet ret=data.insert(std::pair<std::string,Var_Type>(name,def));
            if(ret.second)
                return &(ret.first->second);
            else return NULL;
        }
        else
        {
            return &(it->second);
        }
    }

    /** function get_var() return the value found in the map,\sa enter.
     */
    inline Var_Type get_var(const std::string& name,const Var_Type& def)
    {
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
        DataIter it;
        it=data.find(name);
        if(it==data.end())
        {
            return def;
        }
        else
        {
            return it->second;
        }
    }

    /** this function can be used to assign or get the var use corrospond name,\sa enter.
     */
    inline Var_Type& operator[](const std::string& name)
    {
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
        DataIter it;
        it=data.find(name);
        if(it==data.end())
        {
            Var_Type def;
            while(1)
            {
                InsertRet ret=data.insert(std::pair<std::string,Var_Type>(name,def));
                if(ret.second)
                    return (ret.first->second);
            }
//            else return def;//UNSAFE!!!!!
        }
        else
        {
            return it->second;
        }
    }

    std::string getStatsAsText()
    {
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
        std::ostringstream ost;

        for(DataIter it=data.begin();it!=data.end();it++)
            ost<<std::setw(39)<<std::setiosflags(std::ios::left)<<it->first<<"  "
              <<std::setw(39)<<std::setiosflags(std::ios::left)<<it->second<<std::endl;
        std::string a;
        return ost.str();
    }

    /** this print all the names with var to the screen,\sa enter.
     */
    void dumpAllVars(const size_t column_width=80)
    {
        std::cout<<std::endl<<getStatsAsText(column_width);
    }

protected:
    std::map<std::string,Var_Type> data;

#ifdef MUTI_THREAD
    pi::Mutex            mMutex;
#endif
};//end of class SvarWithType

typedef SvarWithType<int>           SInt;
typedef SvarWithType<double>        SDouble;
typedef SvarWithType<std::string>   SString;

class Svar
{
public:

    typedef std::map<std::string,std::string>           SvarMap;
    typedef std::map<std::string,std::string>::iterator SvarIter;

    enum SVARMODE {
        SILENT      = 0x00,
        UPDATE      = 0x01,
        VERBOSE     = 0x02,
        OVERWRITE   = 0x04
    };

    enum PARSEMODE{
        DEFAULT_CMD1    = 0,
        DEFAULT_CMD2    = 1,
        CMD1            = 2,
        CMD_2           = 3
    };

public:
    Svar();
    ~Svar();

    /** This gives us singletons instance. \sa enter */
    static inline Svar& instance()
    {
        static Svar* inst=0;

        if(!inst)
        {
            inst = new Svar();
        }
        return *inst;
    }
    /** \brief update svar
     */
    bool insert(std::string name, std::string var, bool overwrite=true);
    std::string expandVal(std::string val,char flag='{');
    bool setvar(std::string s);//eg. setvar("var=val");
    std::string getvar(std::string name);

    bool ParseLine(std::string s,bool bSilentFailure=false);
    bool ParseStream(std::istream& is);
    bool ParseFile(std::string sFileName);

    bool ParseMain(int argc,char** argv,PARSEMODE mode=DEFAULT_CMD1);

    /** \brief
     */
    bool exist(const std::string& name);
    template <class T> T get_var(const std::string& name,const T& def);

    int&    GetInt(std::string name, int defaut=0, SVARMODE mode=SILENT);
    double& GetDouble(std::string name, double defaut, SVARMODE mode=SILENT);
    std::string& GetString(std::string name, std::string defaut, SVARMODE mode=SILENT);

    bool erase(const std::string& name);
    void update();
    const SvarMap& get_data();

    /** \brief clear Svar data
    */
    void clear();

    /** \brief clear all data including SvarWithType<int>, SvarWithType<double>, SvarWithType<string>
    */
    void clearAll();

    /** \brief other utils
     */
    std::string getStatsAsText();
    void dumpAllVars();

    bool save2file(std::string filename="");
public:
    SvarWithType<int>       i;
    SvarWithType<double>    d;
    SvarWithType<std::string>    s;

protected:
#ifdef MUTI_THREAD
    pi::Mutex            mMutex;
#endif
    SvarMap              data;
};//end of class Svar


template <class T>
T Svar::get_var(const std::string& name,const T& def)
{

#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
    SvarWithType<T>& typed_map=SvarWithType<T>::instance();
    T* ptr=typed_map.get_ptr(name);
    if(ptr)
        return *ptr;
        //First: Use the var from SvarWithType
    SvarIter it;
    it=data.find(name);
    if(it==data.end())
    {
        return *(typed_map.get_ptr(name,def));
        //Third: Both did not get the var need,insert defaut var to SvarWithType
    }
    else
    {
        std::string& str_var=it->second;
        std::istringstream istr_var(str_var);
        T var;
        istr_var>>var;
        typed_map.insert(name,var);
        //Second: Found the var in Svar,insert to SvarWithType to accelarate
        return var;
    }
}


// FIXME: Going to remove
int*    SvarInt(std::string name,int def);
double* SvarDouble(std::string name,double def);
std::string* SvarString(std::string name,std::string def);

std::string UncommentString(std::string s);
std::vector<std::string> ChopAndUnquoteString(std::string s);
#endif // SVAR_H

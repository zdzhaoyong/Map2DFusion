#include <vector>
#include <iostream>
#include <set>
#include <pthread.h>

#include "Scommand.h"

using namespace std;

namespace pi
{

template<class C> class ThreadLocal
{
private:
    pthread_key_t key;

    static void deleter(void* v)
    {
        delete static_cast<C*>(v);
    }

public:
    ThreadLocal()
    {
        pthread_key_create(&key, deleter);
        pthread_setspecific(key, new C);
    }

    ~ThreadLocal()
    {
        deleter(pthread_getspecific(key));
        pthread_setspecific(key, 0);
        pthread_key_delete(key);
    }


    C& operator()()
    {
        return *static_cast<C*>(pthread_getspecific(key));
    }
};

template<class A, class B> class MutexMap
{
private:
    map<A, B> _map;
    pthread_mutex_t mutex;

public:
    MutexMap()
    {
        pthread_mutex_init(&mutex, 0);
    }

    ~MutexMap()
    {
        pthread_mutex_destroy(&mutex);
    }

    B get(const A& a)
    {
        B b;
        pthread_mutex_lock(&mutex);
        b = _map[a];
        pthread_mutex_unlock(&mutex);
        return b;
    }

    void set(const A&a, const B& b)
    {
        pthread_mutex_lock(&mutex);
        _map[a] = b;
        pthread_mutex_unlock(&mutex);
    }
};


class GUI_language
{

public:
    GUI_language():scommand_(Scommand::instance())
    {
        scommand_.RegisterCommand(".", collect_lineCB, this);
        scommand_.RegisterCommand("function", functionCB, this);
        scommand_.RegisterCommand("endfunction", endfunctionCB, this);
        scommand_.RegisterCommand("if", gui_if_equalCB, this);
        scommand_.RegisterCommand("else", gui_if_elseCB, this);
        scommand_.RegisterCommand("endif", gui_endifCB, this);
    }

    ~GUI_language()
    {
    }


private:
    pthread_mutex_t  functionlist_mutex;

    ThreadLocal<string> current_function, if_gvar, if_string;
    ThreadLocal<vector<string> > collection, ifbit, elsebit;
    MutexMap<string, vector<string> > functions;

    static GUI_language& C(void* v)
    {
        return *static_cast<GUI_language*>(v);
    }


#define CallBack(X) static void X##CB(void* t, string a, string b){C(t).X(a, b);}

    CallBack(collect_line);
    void collect_line(string, string l)
    {
        collection().push_back(l);
    }



    CallBack(function);
    void function(string name, string args)
    {
        svar.GetInt("Svar.Collecting",0)++;
        vector<string> vs = ChopAndUnquoteString(args);
        if(vs.size() != 1)
        {
            cerr << "Error: " << name << " takes 1 argument: " << name << " name\n";
            return;
        }

        current_function()=vs[0];
        collection().clear();
    }

    CallBack(endfunction)
    void endfunction(string name, string args)
    {
        svar.GetInt("Svar.Collecting",0)--;
        if(current_function() == "")
        {
            cerr << "Error: " << name << ": no current function.\n";
            return;
        }

        vector<string> vs = ChopAndUnquoteString(args);
        if(vs.size() != 0)
            cerr << "Warning: " << name << " takes 0 arguments.\n";

        functions.set(current_function(), collection());

        scommand_.RegisterCommand(current_function(), runfuncCB, this);

        current_function().clear();
        collection().clear();
    }

    CallBack(runfunc)
    void runfunc(string name, string /*args*/)
    {
        vector<string> v = functions.get(name);
        for(unsigned int i=0; i < v.size(); i++)
            svar.ParseLine(v[i]);
    }


    CallBack(gui_if_equal)
    void gui_if_equal(string name, string s)
    {
        svar.GetInt("Svar.Collecting",0)++;
        bool is_equal=false;
        string::size_type n;
        n=s.find("=");
        if(n != string::npos)
        {
            string left = s.substr(0, n);
            string right = s.substr(n+1);
            //Strip whitespace from around left;
            string::size_type s=0, e = left.length()-1;
            if('!'==left[e])
            {
                //                        cout<<"Found !"<<endl;
                e--;
                is_equal=true;
            }
            for(; isspace(left[s]) && s < left.length(); s++)
            {}
            if(s==left.length()) // All whitespace before the `='?
                left="";
            else
                for(; isspace(left[e]); e--){}
            if(e >= s)
            {
                left = left.substr(s, e-s+1);
            }
            else left="";

            //Strip whitespace from around val;
            s = 0, e = right.length() - 1;
            for(; isspace(right[s]) && s < right.length(); s++)
            {}
            if( s < right.length())
            {
                for(; isspace(right[e]); e--)
                {}
                right = right.substr(s, e-s+1);
            }
            else right = "";

            //                    cout<<"Found =,Left:-"<<left<<"-,Right:-"<<right<<"-\n";

            if(left==right) is_equal=!is_equal;
        }
        else if(s!="")
        {
            is_equal=true;
        }

        collection().clear();
        if(is_equal)
            if_gvar() = "";
        else if_gvar()= "n";
        if_string() = "";
    }


    CallBack(gui_if_else)
    void gui_if_else(string /*name*/, string /*args*/)
    {
        ifbit() = collection();
        if(ifbit().empty())
            ifbit().push_back("");
        collection().clear();
    }

    CallBack(gui_endif)
    void gui_endif(string /*name*/, string /*args*/)
    {
        svar.GetInt("Svar.Collecting",0)--;
        if(ifbit().empty())
            ifbit() = collection();
        else
            elsebit() = collection();

        collection().clear();

        //Save a copy, since it canget trashed
        vector<string> ib = ifbit(), eb = elsebit();
        string gv = if_gvar(), st = if_string();

        ifbit().clear();
        elsebit().clear();
        if_gvar().clear();
        if_string().clear();
        //                cout<<"SvarName="<<gv<<",Value="<<svar.GetString(gv,"")<<",Test="<<st<<endl;
        if(gv == st)
            for(unsigned int i=0; i < ib.size(); i++)
                svar.ParseLine(ib[i]);
        else
            for(unsigned int i=0; i < eb.size(); i++)
                svar.ParseLine(eb[i]);
    }
    Scommand& scommand_;
};

GUI_language GUI_language_instance;

}

#include <fstream>
#include <vector>

#include "Svar.h"
#include "base/utils/utils_str.h"
#include "base/system/file_path/file_path.h"
#include "base/debug/debug_config.h"
#include <deque>

#include "base/Svar/Scommand.h"

using namespace std;

//Svar svar;

int* SvarInt(string name,int def)
{
    def=svar.GetInt(name,def);
    return svar.i.get_ptr(name,def);
}

double* SvarDouble(string name,double def)
{
    def=svar.GetDouble(name,def);
    return svar.d.get_ptr(name,def);
}

string* SvarString(string name,string def)
{
    def=svar.GetString(name,def);
    return svar.s.get_ptr(name,def);
}

inline std::string Trim(const std::string& str, const std::string& delimiters = " \f\n\r\t\v" )
{
    const size_t f = str.find_first_not_of( delimiters );
    return f == std::string::npos ?
                "" :
                str.substr( f, str.find_last_not_of( delimiters ) + 1 );
}

// Find the open brace preceeded by '$'
const char* FirstOpenBrace(const char* str,char flag='{')
{
    bool symbol = false;

    for(; *str != '\0'; ++str ) {
        if( *str == '$') {
            symbol = true;
        }else{
            if( symbol ) {
                if( *str == flag ) {
                    return str;
                } else {
                    symbol = false;
                }
            }
        }
    }
    return 0;
}

// Find the first matching end brace. str includes open brace
const char* MatchingEndBrace(const char* str,char flag='{')
{
    char endflag='}';
    if(flag=='(') endflag=')';
    else if(flag=='[') endflag=']';
    int b = 0;
    for(; *str != '\0'; ++str ) {
        if( *str == flag ) {
            ++b;
        }else if( *str == endflag ) {
            --b;
            if( b == 0 ) {
                return str;
            }
        }
    }
    return 0;
}

vector<string> ChopAndUnquoteString(string s)
{
    vector<string> v;
    string::size_type nPos=0;
    string::size_type nLength = s.length();
    while(1)
    {
        string sTarget;
        char cDelim;
        // Get rid of leading whitespace:
        while((nPos<nLength)&&(s[nPos]==' '))
            nPos++;
        if(nPos==nLength)
            return v;

        // First non-whitespace char...
        if(s[nPos]!='\"')
            cDelim=' ';
        else
        {
            cDelim = '\"';
            nPos++;
        }
        for (; nPos < nLength; ++nPos) {
            char c = s[nPos];
            if (c == cDelim)
                break;
            if (cDelim == '"' && nPos+1<nLength && c == '\\') {
                char escaped = s[++nPos];
                switch (escaped) {
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case 't': c = '\t'; break;
                default: c = escaped; break;
                }
            }
            sTarget+=c;
        }
        v.push_back(sTarget);

        if(cDelim=='\"')
            nPos++;
    }
}

string::size_type FindCloseBrace(const string& s, string::size_type start, char op, char cl)
{
  string::size_type open=1;
  string::size_type i;

  for(i=start+1; i < s.size() ; i++)
    {
  if(s[i] == op)
    open++;
  else if(s[i] == cl)
    open--;

  if(open == 0)
    break;
    }

  if(i == s.size())
    i = s.npos;
  return i;
}

string UncommentString(string s)
{
  //int n = s.find("//");
  //return s.substr(0,n);

  int q=0;

  for(string::size_type n=0; n < s.size(); n++)
  {
    if(s[n] == '"')
        q = !q;

    if(s[n] == '/' && !q)
    {
        if(n < s.size() -1 && s[n+1] == '/')
            return s.substr(0, n);
    }
  }

  return s;
}


Svar::Svar()
{

}

Svar::~Svar()
{
    dumpAllVars();
}

void Svar::clear()
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif

    data.clear();
}

void Svar::clearAll()
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif

    data.clear();
}

bool Svar::erase(const string& name)
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif
    return data.erase(name);
}

bool Svar::exist(const string& name)
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif

    SvarIter it=data.find(name);
    return it!=data.end();
}

const Svar::SvarMap& Svar::get_data()
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif

    return data;
}

bool Svar::insert(string name,string var,bool overwrite)//default overwrite
{
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
    SvarIter it;
    it=data.find(name);
    if(it==data.end())
    {
        data.insert(pair<string,string>(name,var));
        return true;
    }
    else
    {
        if(overwrite)
        {
            it->second=var;
            if(i.exist(name)) i[name]=pi::str_to_int(var);
            if(d.exist(name)) d[name]=pi::str_to_double(var);
            if(s.exist(name)) s[name]=var;
        }
        return false;
    }
}

std::string Svar::getvar(std::string name)
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif
    SvarIter it;
    it=data.find(name);
    if(it==data.end())
    {
        return "";
    }
    else
    {
        return it->second;
    }
}

std::string Svar::expandVal(std::string val,char flag)
{
    string expanded = val;

    while(true)
    {
        const char* brace = FirstOpenBrace(expanded.c_str(),flag);
        if(brace)
        {
            const char* endbrace = MatchingEndBrace(brace,flag);
            if( endbrace )
            {
                ostringstream oss;
                oss << std::string(expanded.c_str(), brace-1);

                const string inexpand = expandVal( std::string(brace+1,endbrace),flag);
                if( exist(inexpand) ) {
                    oss << getvar(inexpand);
                }else{
                    MSG_ERROR("Unabled to expand: [%s].\nMake sure it is defined and terminated with a semi-colon.\n", inexpand.c_str() );
                    oss << "#";
                }

                oss << std::string(endbrace+1, expanded.c_str() + expanded.length() );
                expanded = oss.str();
                continue;
            }
        }
        break;
    }

    return expanded;
}

/**
 = overwrite
?= don't overwrite
*/
bool Svar::setvar(string s)
{
    //Execution failed. Maybe its an assignment.
    string::size_type n;
    n=s.find("=");
    bool shouldOverwrite=true;

    if(n != string::npos)
    {
        string var = s.substr(0, n);
        string val = s.substr(n+1);

        //Strip whitespace from around var;
        string::size_type s=0, e = var.length()-1;
        if('?'==var[e]) {e--;shouldOverwrite=false;}
        for(; isspace(var[s]) && s < var.length(); s++)
        {}
        if(s==var.length()) // All whitespace before the `='?
            return false;
        for(; isspace(var[e]); e--)
        {}
        if(e >= s)
        {
            var = var.substr(s, e-s+1);

            //Strip whitespace from around val;
            s = 0, e = val.length() - 1;
            for(; isspace(val[s]) && s < val.length(); s++)
            {}
            if( s < val.length())
            {
                for(; isspace(val[e]); e--)
                {}
                val = val.substr(s, e-s+1);
            }
            else val = "";

            insert(var, (val),shouldOverwrite);
            return true;
        }
    }

    return false;
}

bool Svar::ParseLine(string s,bool bSilentFailure)
{
    if(s == "")
        return 0;
    int& collectFlag=svar.GetInt("Svar.Collecting",0);
    if(collectFlag)
    {
        istringstream ist(s);
        string sCommand;
        ist>>sCommand;
        if(sCommand=="endif"||sCommand=="fi")
            scommand.Call("endif");
        if(sCommand=="else")
            scommand.Call("else");
        else if(sCommand=="endfunction")
            scommand.Call("endfunction");
        else if(sCommand==".")
        {
            scommand.Call(".",ist.str());
        }
        else
            scommand.Call(".",s);
        return 0;
    }
    s = UncommentString(expandVal(s,'{'));
    s = UncommentString(expandVal(s,'('));
    if(s == "")
        return 0;

    // Old ParseLine code follows, here no braces can be left (unless in arg.)
    istringstream ist(s);

    string sCommand;
    string sParams;

    //Get the first token (the command name)
    ist >> sCommand;
    if(sCommand == "")
        return 0;

    //Get everything else (the arguments)...

    //Remove any whitespace
    ist >> ws;
    getline(ist, sParams);

  //Attempt to execute command
    if (Scommand::instance().Call(sCommand,sParams))
        return true;

    if(setvar(s))
        return 1;

    if(!bSilentFailure)
        cerr << "? GUI_impl::ParseLine: Unknown command \"" << sCommand << "\" or invalid assignment." << endl;
    return 0;
}

bool Svar::ParseStream(istream& is)
{
    string parsingFile=svar.GetString("Svar.ParsingFile","");
    pi::StringArray path_file=pi::path_split(parsingFile);
    insert("Svar.ParsingPath",path_file[0],true);
    insert("Svar.ParsingName",path_file[1],true);
    insert("Svar.ParsingFile",parsingFile,true);
    string buffer;
    int& shouldParse=svar.GetInt("Svar.NoReturn",1);
    while (getline(is, buffer)&&shouldParse) {
        // Lines ending with '\' are taken as continuing on the next line.
        while(!buffer.empty() && buffer[buffer.length() - 1] == '\\')
        {
            string buffer2;
            if (! getline(is, buffer2))
                break;
            buffer = buffer.substr(0, buffer.length() - 1) + buffer2;
        }
        ParseLine(buffer);
    }
    shouldParse=1;
    return true;
}


bool Svar::ParseFile(string sFileName)
{
    static deque<string> fileQueue;

    ifstream ifs(sFileName.c_str());

    if(!ifs.is_open())
    {
        cerr << "!Svar::ParseFile: Failed to load script file \"" << sFileName << "\"."<< endl;
        return 0;
    }

    fileQueue.push_back(sFileName);
    svar.GetString("Svar.ParsingFile",sFileName)=sFileName;

    bool ret=ParseStream(ifs);
    ifs.close();

//    cout<<"Finished parsing "<<fileQueue.back();
    fileQueue.pop_back();
    if(fileQueue.size())
    {
//        cout<<"Back to parsing "<<fileQueue.back();
        svar.GetString("Svar.ParsingFile",sFileName)=fileQueue.back();
        string parsingFile=fileQueue.back();
        pi::StringArray path_file=pi::path_split(parsingFile);
        insert("Svar.ParsingPath",path_file[0],true);
        insert("Svar.ParsingName",path_file[1],true);
        insert("Svar.ParsingFile",parsingFile,true);
    }
    else
    {
        svar.erase("Svar.ParsingName");
        svar.erase("Svar.ParsingPath");
        svar.erase("Svar.ParsingFile");
    }
    return ret;
}

bool Svar::ParseMain(int argc, char** argv, PARSEMODE mode)
{
    // save main cmd things
    i["argc"] = argc;
    SvarWithType<char**>::instance()["argv"] = argv;

    // parse main cmd
    for(int i=1; i<argc; i++)
        setvar(argv[i]);

    // parse default config file
    string cfg_File=argv[0];
    insert("argv0", cfg_File);
    pi::StringArray path_name = pi::path_split(cfg_File);
    insert("ProgramPath",path_name[0]);
    insert("ProgramName",path_name[1]);
    cfg_File += ".cfg";
    if( !pi::path_exist(cfg_File.c_str()) )
    {
        cfg_File="./Default.cfg";
    }

    cfg_File = GetString("conf", cfg_File);
    cout << "Parsing file: " << cfg_File << " ...." << endl;
    ParseFile(cfg_File);

    // parse input argument again
    for(int i=1; i<argc; i++)
        setvar(argv[i]);
}

int& Svar::GetInt(std::string name, int def, SVARMODE mode)
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif

    //First: Use the var from SvarWithType, this is a fast operation
    SvarWithType<int>& typed_map=i;

    int* ptr=typed_map.get_ptr(name);
    if(ptr)
        return *ptr;

    SvarIter it;
    it=data.find(name);
    if(it!=data.end()) //Second: Use the var from Svar
    {
        string& str_var=it->second;
        istringstream istr_var(str_var);
        istr_var>>def;
        while(!ptr)
        {
            ptr=(typed_map.get_ptr(name,def));
            if(ptr) break;
        }
        return *ptr;
    }
    else //Third:Both did not get the var need,insert defaut var to SvarWithType
    {
        while(!ptr)
        {
            ptr=(typed_map.get_ptr(name,def));
            if(ptr) break;
        }
        if( mode & UPDATE )
            insert(name,pi::itos(def));
        return *ptr;
    }
}

double& Svar::GetDouble(std::string name, double def, SVARMODE mode)
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif

    //First: Use the var from SvarWithType, this is a fast operation
    SvarWithType<double>& typed_map=d;

    double* ptr=typed_map.get_ptr(name);
    if(ptr)
        return *ptr;

    SvarIter it;
    it=data.find(name);
    if(it!=data.end()) //Second: Use the var from Svar
    {
        string& str_var=it->second;
        istringstream istr_var(str_var);
        istr_var>>def;
        while(!ptr)
        {
            ptr=(typed_map.get_ptr(name,def));
            if(ptr) break;
        }
        return *ptr;
    }
    else //Third:Both did not get the var need,insert defaut var to SvarWithType
    {
        while( !ptr )
        {
            ptr = typed_map.get_ptr(name,def);
            if(ptr) break;
        }

        if( mode & UPDATE )
            insert(name, pi::dtos(def, 12));

        return *ptr;
    }
}

string& Svar::GetString(std::string name, std::string def, SVARMODE mode)
{
#ifdef MUTI_THREAD
    pi::ScopedMutex lock(mMutex);
#endif

    //First: Use the var from SvarWithType, this is a fast operation
    SvarWithType<string>& typed_map=s;

    string* ptr=typed_map.get_ptr(name);
    if(ptr)
        return *ptr;

    SvarIter it;
    it=data.find(name);
    if(it!=data.end()) //Second: Use the var from Svar
    {
        string& def=it->second;
        while(!ptr)
        {
            ptr=(typed_map.get_ptr(name,def));
            if(ptr) break;
        }
        return *ptr;
    }
    else //Third:Both did not get the var need,insert defaut var to SvarWithType
    {
        while(!ptr)
        {
            ptr=(typed_map.get_ptr(name,def));
            if(ptr) break;
        }
        if( mode & UPDATE )
            insert(name,def);

        return *ptr;
    }
}

void Svar::update()
{
    //from i
    SvarWithType<int>::DataMap data_i=i.get_data();
    for(SvarWithType<int>::DataIter it=data_i.begin();it!=data_i.end();it++)
    {
        const string& name=it->first;
        if(!exist(name)) continue;
        insert(name,pi::itos(it->second),true);
    }

    //from d
    SvarWithType<double>::DataMap data_d=d.get_data();
    for(SvarWithType<double>::DataIter it=data_d.begin();it!=data_d.end();it++)
    {
        const string& name=it->first;
        if(!exist(name)) continue;
        insert(name,pi::dtos(it->second, 12));
    }

    //from s
    SvarWithType<string>::DataMap data_s=s.get_data();
    for(SvarWithType<string>::DataIter it=data_s.begin();it!=data_s.end();it++)
    {
        const string& name=it->first;
        if(!exist(name)) continue;
        insert(name,it->second);
    }
}

bool Svar::save2file(std::string filename)
{
   if(filename.size()==0) filename=GetString("Config_File","Default.cfg");
   ofstream ofs(filename.c_str());
   if(!ofs.is_open()) return 0;

   SvarMap data_copy;
   {
#ifdef MUTI_THREAD
       pi::ScopedMutex lock(mMutex);
#endif
       data_copy=data;
   }
   for(SvarIter it=data_copy.begin();it!=data_copy.end();it++)
   {
       ofs<<it->first<<" = "<<it->second<<endl;
   }
}

std::string Svar::getStatsAsText()
{
#ifdef MUTI_THREAD
        pi::ScopedMutex lock(mMutex);
#endif
    if(data.size()+i.data.size()+d.data.size()+s.data.size()==0) return "";
    ostringstream ost;
    string str;

    ost<<"================================== Svar report ===============================\n";
    ost<<"NAME                                     VALUE                                \n";
    str=i.getStatsAsText();
    if(str!="")
    ost<<"------------------------------------------------------------------------------\n"<<str;
    str=d.getStatsAsText();
    if(str!="")
    ost<<"------------------------------------------------------------------------------\n"<<str;
    str=s.getStatsAsText();
    if(str!="")
    ost<<"------------------------------------------------------------------------------\n"<<str;
    if(data.size()){
    ost<<"------------------------------------------------------------------------------\n";
    for(SvarIter it=data.begin();it!=data.end();it++)
        ost<<setw(39)<<setiosflags(ios::left)<<it->first<<"  "
         <<setw(39)<<setiosflags(ios::left)<<it->second<<endl;
    }
    ost<<"=============================== End of Svar report ===========================\n";

    return ost.str();
}

void Svar::dumpAllVars()
{
    cout<<endl<<getStatsAsText();
}

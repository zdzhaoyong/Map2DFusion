#include "Scommand.h"
#include "base/utils/utils_str.h"

#include <stdlib.h>

using namespace std;

void buildInHandle(void* ptr, string command, string sParams)
{
    if(command=="include"||command=="parse")
    {
        Svar* svar_ptr=(Svar*)ptr;
        svar_ptr->ParseFile(sParams);
    }
    else if(command=="echo")
    {
        cout<<sParams<<endl;
    }
    else if(command=="GetString")
    {
        Svar* svar_ptr=(Svar*)ptr;
        svar_ptr->insert(sParams,svar_ptr->GetString(sParams,""));
    }
    else if(command=="GetInt")
    {
        Svar* svar_ptr=(Svar*)ptr;
        svar_ptr->insert(sParams,pi::itos(svar_ptr->GetInt(sParams,0)));
    }
    else if(command=="GetDouble")
    {
        Svar* svar_ptr=(Svar*)ptr;
        svar_ptr->insert(sParams,pi::dtos(svar_ptr->GetDouble(sParams,0)));
    }
}

void systemFunction(void* ptr,string command,string sParams)
{
    system(sParams.c_str());
}

Scommand::Scommand():data(SvarWithType<CallbackVector>::instance())
{
    RegisterCommand("include",buildInHandle,&svar);
    RegisterCommand("parse",buildInHandle,&svar);
    RegisterCommand("echo",buildInHandle,&svar);
    RegisterCommand("GetVar",buildInHandle,&svar);
    RegisterCommand("GetInt",buildInHandle,&svar);
    RegisterCommand("GetDouble",buildInHandle,&svar);
    RegisterCommand("GetString",buildInHandle,&svar);
    RegisterCommand("system",systemFunction);
}

Scommand& Scommand::instance()
{
    static Scommand* inst = 0;
    if(!inst)
        inst = new Scommand;
    return *inst;
}

void Scommand::RegisterCommand(string sCommandName, CallbackProc callback, void* thisptr)
{
    CallbackVector& calls=data[sCommandName];
    calls.push_back(CallbackInfoStruct(callback,thisptr));
}

void Scommand::UnRegisterCommand(string sCommandName)
{
    CallbackVector& calls=data[sCommandName];
    calls.clear();
}

void Scommand::UnRegisterCommand(string sCommandName,void* thisptr)
{
    CallbackVector& calls=data[sCommandName];
    for(int i = static_cast<int>(calls.size()) - 1; i>=0; i--)
      if(calls[i].thisptr == thisptr)
        calls.erase(calls.begin() + i);
}

void Scommand::UnRegisterCommand(void* thisptr)
{
    map<string,CallbackVector>&mmCallBackMap=*(data.get_ptr());
    for(map<string, CallbackVector>::iterator i=mmCallBackMap.begin(); i!=mmCallBackMap.end(); i++)
        UnRegisterCommand(i->first, thisptr);
}


bool Scommand::Call(std::string sCommand, std::string sParams)
{
    if(!data.exist(sCommand)) return false;
    CallbackVector& calls=data[sCommand];
    for(CallbackVector::iterator it=calls.begin();it!=calls.end();it++)
        it->cbp(it->thisptr, sCommand, sParams);
    return true;
}

/** split the command and paraments from a string
 * eg:Call("shell ls"); equal Call("shell","ls");
 */
bool Scommand::Call(const std::string& sCommand)
{
    unsigned int found=sCommand.find_first_of(" ");
//    cout<<"sCommand="<<sCommand<<"\nFound="<<found<<"\nCommand="<<sCommand.substr(0,found)<<"\nParaments="<<sCommand.substr(found+1);
    if(found<sCommand.size())
        return Call(sCommand.substr(0,found),sCommand.substr(found+1));
    else
        return Call(sCommand,"");
}

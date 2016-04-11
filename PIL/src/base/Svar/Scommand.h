#ifndef SCOMMAND_H
#define SCOMMAND_H

#include <vector>

#include "Svar.h"
#define scommand Scommand::instance()

///
/// ptr      - class pointer
/// sCommand - command string
/// sParams  - parameters
///
typedef void (*CallbackProc)(void* ptr, std::string sCommand, std::string sParams);


struct CallbackInfoStruct
{
  CallbackInfoStruct(CallbackProc callback,void* ptr):cbp(callback),thisptr(ptr){}

  void Call(std::string sCommand="", std::string sParams=""){cbp(thisptr, sCommand, sParams);}

  CallbackProc cbp;
  void* thisptr;
};

typedef std::vector<CallbackInfoStruct> CallbackVector;


class Scommand
{
public:
    Scommand();
    static Scommand& instance();

    void RegisterCommand(std::string sCommandName, CallbackProc callback, void* thisptr=NULL);
    void UnRegisterCommand(std::string sCommandName);
    void UnRegisterCommand(std::string sCommandName,void* thisptr);
    void UnRegisterCommand(void* thisptr);

    bool Call(std::string sCommand, std::string sParams);
    bool Call(const std::string& sCommand);

protected:
    SvarWithType<CallbackVector> &data;
};

#endif // SCOMMAND_H

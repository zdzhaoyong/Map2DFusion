#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>

#include <signal.h>
#include <execinfo.h>
#include <errno.h>
#include <cxxabi.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>

#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <assert.h>
#include <inttypes.h>
//#include <tmmintrin.h>

#include <string>
#include <vector>
#include <algorithm>

#include "base/Svar/Svar.h"
#include "base/utils/utils_str.h"
#include "debug_config.h"

using namespace std;


namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// debug level 
static int  g_iDebugLevel = 4;

// debug level stack
#define DEBUG_LEVE_STACK_SIZE 128

static int  g_aDebugLevelStack[DEBUG_LEVE_STACK_SIZE];
static int  g_iDebugLevelStackIdx=0;


/** 
 *  Set level of debug print
 *
 *  Parameters:
 *      \param[in]  i       level
 */
void dbg_set_level(int i)
{
    g_iDebugLevel = i;
    svar.GetInt("DBG_LEVEL", 4) = g_iDebugLevel;
} 

/**
 *  Get current debug level
 *
 *  Return value:
 *      current debug level
 */
int  dbg_get_level(void)
{
    g_iDebugLevel = svar.GetInt("DBG_LEVEL", 4);
    return g_iDebugLevel;
}

/**
 *  Push a new debug level to stack
 *
 *  FIXME:
 *      Not implementated yet
 *
 *  Paramters:
 *      \param[in]  i       new debug level
 *  Return value:
 *      None
 */
void dbg_push_level(int level)
{
    if( g_iDebugLevelStackIdx >= DEBUG_LEVE_STACK_SIZE ) {
        dbg_pe("Debug level stack overfull!");
        return;
    }

    g_aDebugLevelStack[g_iDebugLevelStackIdx++] = g_iDebugLevel;
    g_iDebugLevel = level;
    svar.GetInt("DBG_LEVEL", 4) = g_iDebugLevel;
}

/**
 *  Pop top debug level from stack
 *
 *  FIXME:
 *      Not implementated yet
 *
 *  Paramters:
 *      None
 *  Return value:
 *      new debug level
 */
int  dbg_pop_level(void)
{
    if( g_iDebugLevelStackIdx <= 0 ) {
        dbg_pe("Debug level stack is empty!");
        return g_iDebugLevel;
    }

    g_iDebugLevel = g_aDebugLevelStack[--g_iDebugLevelStackIdx];
    svar.GetInt("DBG_LEVEL", 4) = g_iDebugLevel;

    return g_iDebugLevel;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef std::map<std::string, LogInfo_Message_Handle> LogInfoMessageHandleMap;
LogInfoMessageHandleMap g_logInfoMsgHandleMap;


int dbg_registMessageHandle(const std::string &handleName, LogInfo_Message_Handle &msgHandle)
{
    LogInfoMessageHandleMap::iterator it;

    it = g_logInfoMsgHandleMap.find(handleName);
    if( it == g_logInfoMsgHandleMap.end() ) {
        g_logInfoMsgHandleMap.insert(make_pair(handleName, msgHandle));
    } else {
        it->second = msgHandle;
    }

    return 0;
}

int dbg_unregistMessageHandle(const std::string &handleName)
{
    LogInfoMessageHandleMap::iterator it;

    it = g_logInfoMsgHandleMap.find(handleName);
    if( it == g_logInfoMsgHandleMap.end() ) {
        return -1;
    } else {
        g_logInfoMsgHandleMap.erase(it);
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

char *_str_cat(char *s_out, const char *s1, const char *s2, const char *s3)
{
    int     i, j, l1, l2, l3;

    l1 = strlen(s1);
    l2 = strlen(s2);
    l3 = strlen(s3);

    j = 0;
    for(i=0; i<l1; i++) s_out[j++] = s1[i];
    for(i=0; i<l2; i++) s_out[j++] = s2[i];
    for(i=0; i<l3; i++) s_out[j++] = s3[i];
    s_out[j] = 0;

    return s_out;
}

void dbg_printf(int level,
               const char *fname, int line, const char *func,
               const char *szFmtString, ...)
{
    #define MAX_DBUG_BUFF_LEN    4096

    char    sHeader[MAX_DBUG_BUFF_LEN];
    char    sTail[MAX_DBUG_BUFF_LEN];

    char    sHeader2[MAX_DBUG_BUFF_LEN];
    char    sTail2[MAX_DBUG_BUFF_LEN];

    char    sBuf1[MAX_DBUG_BUFF_LEN];
    char    sBuf2[MAX_DBUG_BUFF_LEN];

    int     lBuf1, lBuf2;

    va_list va_params;

    // check debug level
    if( level > svar.GetInt("DBG_LEVEL", 4) ) return;

    // alloc string buffer
    lBuf1 = strlen(szFmtString);

    // generate header, tail
    if( level == 1 ) {
        sprintf(sHeader, "\033[31mERR:\033[0m  ");
        sprintf(sTail,   "      \033[35m(LINE: %5d, FILE: %s, FUNC: %s)\033[0m", line, fname, func);

        sprintf(sHeader2, "ERR:  ");
        sprintf(sTail2,   "");
    } else if (level == 2 ) {
        sprintf(sHeader, "\033[33mWARN:\033[0m ");
        sprintf(sTail,   "      \033[35m(LINE: %5d, FILE: %s, FUNC: %s)\033[0m", line, fname, func);

        sprintf(sHeader2, "WARN: ");
        sprintf(sTail2,   "");
    } else if (level == 3 ) {
        sprintf(sHeader, "\033[36mINFO:\033[0m ");
        sprintf(sTail,   "      \033[35m(LINE: %5d, FILE: %s, FUNC: %s)\033[0m", line, fname, func);

        sprintf(sHeader2, "INFO: ");
        sprintf(sTail2,   "");
    } else if (level == 4 ) {
        sprintf(sHeader, "\033[34m%s\033[0m >> ", func);
        sprintf(sTail, "");

        sprintf(sHeader2, "TRAC: ");
        sprintf(sTail2,   "");
    } else {
        sprintf(sHeader,"");
        sprintf(sTail, "");

        sprintf(sHeader2, "");
        sprintf(sTail2,   "");
    }

    // generate format string
    va_start(va_params, szFmtString);
    vsprintf(sBuf1, szFmtString, va_params);
    va_end(va_params);

    lBuf1 = strlen(sBuf1);
    if( lBuf1 > 0 && level < 4 )
        if( sBuf1[lBuf1-1] != '\n' ) {
            sBuf1[lBuf1] = '\n';
            sBuf1[lBuf1+1] = 0;
        }

    // concatenate final string
    _str_cat(sBuf2, sHeader, sBuf1, sTail);

    lBuf2 = strlen(sBuf2);
    if( lBuf2 > 0 ) {
        if( sBuf2[lBuf2-1] == '\n' ) sBuf2[lBuf2-1] = 0;
    }

    // output message
    puts(sBuf2);

    // output log information to registed call-back functions
    if( g_logInfoMsgHandleMap.size() > 0 ) {
        // concatenate final string
        _str_cat(sBuf2, sHeader2, sBuf1, sTail2);

        lBuf2 = strlen(sBuf2);
        if( lBuf2 > 0 ) {
            if( sBuf2[lBuf2-1] == '\n' ) sBuf2[lBuf2-1] = 0;
        }

        StringArray sa = split_line(sBuf2);

        // call call-back function
        for(LogInfoMessageHandleMap::iterator it=g_logInfoMsgHandleMap.begin(); it!=g_logInfoMsgHandleMap.end(); it++) {
            for(int i=0; i<sa.size(); i++) it->second(sa[i]);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

pid_t popen2(const char *command, int *infp, int *outfp)
{
    #define READ    0
    #define WRITE   1

    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0) {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);

        execl("/bin/sh", "sh", "-c", command, NULL);
        perror("execl");
        exit(1);
    }

    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

    if (outfp == NULL)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];

    return pid;
}

int get_exec_output(char *cmd, char *buf, int buf_len)
{
    pid_t   pid;
    int     stat_loc;
    int     infp, outfp;
    int     i, l;
    ssize_t nread;

    // open pipe
    pid = popen2(cmd, &infp, &outfp);
    if ( pid <= 0) {
        printf("ERR: Unable to exec given program\n");
        exit(1);
    }

    // read output
    nread = read(outfp, buf, buf_len);

    // close pip file
    close(infp);
    close(outfp);

    // wait child process finished
    wait(&stat_loc);

    // process output buffer
    l = strlen(buf);

    // get first line
    for(i=0; i<l; i++) {
        if( buf[i] == '\n' || buf[i] == '\r' )
            buf[i] = '\0';
    }

    // normal method remove trailing \r
    /*
    if( buf[l-1] == '\n' || buf[l-1] == '\r' )
        buf[l-1] = '\0';
    */

    return 0;
}

static inline void printStackTrace( FILE *out = stderr, unsigned int max_frames = 100 )
{
    fprintf(out, "stack trace:\n");

    // storage array for stack trace address data
    void    *addrlist[max_frames+1];

    size_t  funcnamesize = 1024;
    char    funcname[1024];
    char    s_addr[200];
    char    s_off[200];
    char    s_cmd[1024];
    char    s_fileline[1024];

    unsigned int    i, j;

    // retrieve current stack addresses
    unsigned int addrlen = backtrace( addrlist, sizeof( addrlist ) / sizeof( void* ));

    if ( addrlen == 0 ) {
        fprintf( out, "  \n" );
        return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // Actually it will be ## program address function + offset
    // this array must be free()-ed
    char** symbollist = backtrace_symbols( addrlist, addrlen );


    // iterate over the returned symbol lines. skip the first 3 line, last line
    //      it is the address of this function.
    for ( i = 3; i < addrlen-1; i++ ) {
        char* begin_name   = NULL;
        char* begin_offset = NULL;
        char* end_offset   = NULL;

        // find parentheses and +address offset surrounding the mangled name
#ifdef DARWIN
        // OSX style stack trace
        for ( char *p = symbollist[i]; *p; ++p ) {
            if (( *p == '_' ) && ( *(p-1) == ' ' ))
                begin_name = p-1;
            else if ( *p == '+' )
                begin_offset = p-1;
        }

        if ( begin_name && begin_offset && ( begin_name < begin_offset )) {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():
            int status;
            char* ret = abi::__cxa_demangle( begin_name, &funcname[0],
                    &funcnamesize, &status );
            if ( status == 0 ) {
                funcname = ret; // use possibly realloc()-ed string
                fprintf( out, "  %-30s %-40s %s\n",
                         symbollist[i], funcname, begin_offset );
            } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                fprintf( out, "  %-30s %-38s() %s\n",
                         symbollist[i], begin_name, begin_offset );
            }

#else // !DARWIN - but is posix


        // not OSX style

        s_addr[0] = '\0';
        s_off[0]  = '\0';

        // ./module(function+0x15c) [0x8048a6d]
        for ( char *p = symbollist[i]; *p; ++p ) {
            if ( *p == '(' )
                begin_name = p;
            else if ( *p == '+' )
                begin_offset = p;
            else if ( *p == '[' && ( begin_offset || begin_name ) )
                end_offset = p;
        }

        // get address string
        if ( end_offset ) {
            for(char *p=end_offset, j=0; *p; ++p) {
                if( *p == '[' )
                    continue;
                else if( *p == ']' )
                    s_addr[j] = '\0';
                else
                    s_addr[j++] = *p;
            }
            //fprintf(out, "addr: %s\n", s_addr);
        }

        // get offset address
        if( begin_offset ) {
            for(char *p=begin_offset, j=0; *p; ++p) {
                if( *p == '+' )
                    continue;
                else if( *p == ')' ) {
                    s_off[j] = '\0';
                    break;
                } else
                    s_off[j++] = *p;
            }
            //fprintf(out, "offset: %s\n", s_off);
        }

        if ( begin_name && end_offset && ( begin_name < end_offset )) {
            *begin_name++   = '\0';
            *end_offset++   = '\0';

            if ( begin_offset )
                *begin_offset++ = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status = 0;
            char* ret = abi::__cxa_demangle( begin_name, funcname,
                                             &funcnamesize, &status );
            char* fname = begin_name;
            if ( status == 0 )
                fname = ret;

            if ( begin_offset ) {
                fprintf( out, "  %s [ \033[31m%s\033[0m + %s ] [%s]\n",
                         symbollist[i], fname, s_off, s_addr );
            } else {
                fprintf( out, "  %s [ %s   %s ] [%s]\n",
                         symbollist[i], fname, "", s_addr );
            }

            // print source file and line no.
            sprintf(s_cmd, "addr2line -e %s %s", symbollist[i], s_addr);
            get_exec_output(s_cmd, s_fileline, 1024);
            fprintf(out, "      \033[32m%s\033[0m\n", s_fileline);

#endif  // !DARWIN - but is posix
        } else {
            // couldn't parse the line? print the whole line.
            fprintf(out, "  %s\n", symbollist[i]);
        }
    }

    free(symbollist);
}

void abortHandler( int signum )
{
    // associate each signal with a signal name string.
    const char* name = NULL;

    // get signal name
    switch( signum )
    {
    case SIGABRT: name = "SIGABRT";  break;
    case SIGSEGV: name = "SIGSEGV";  break;
    case SIGBUS:  name = "SIGBUS";   break;
    case SIGILL:  name = "SIGILL";   break;
    case SIGFPE:  name = "SIGFPE";   break;
    }

    // notify the user which signal was caught. We use printf, because this is the most
    // basic output function. Once you get a crash, it is possible that more complex output
    // systems like streams and the like may be corrupted. So we make the most basic call
    // possible to the lowest level, most standard print function.
    printf("\n");
    printf("-------------------------------------------------------------------------------------------\n");
    if ( name )
        fprintf( stderr, "Caught signal %d (%s)\n", signum, name );
    else
        fprintf( stderr, "Caught signal %d\n", signum );

    // Dump a stack trace
    printStackTrace();

    printf("-------------------------------------------------------------------------------------------\n");

    // If you caught one of the above signals, it is likely you just
    // want to quit your program right now.
    exit( signum );
}


int dbg_stacktrace_setup(void)
{
    signal( SIGABRT, abortHandler );
    signal( SIGSEGV, abortHandler );
    signal( SIGBUS,  abortHandler );
    signal( SIGILL,  abortHandler );
    signal( SIGFPE,  abortHandler );
}


} // end of namespace pi


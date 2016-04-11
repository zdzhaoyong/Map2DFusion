#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <map>

#include <tr1/functional>


namespace pi {

#ifndef DEBUG_LEVEL
    #define DEBUG_LEVEL 4
#endif

////////////////////////////////////////////////////////////////////////////////
/// debug stack trace functions
////////////////////////////////////////////////////////////////////////////////
int dbg_stacktrace_setup(void);

////////////////////////////////////////////////////////////////////////////////
/// ASSERT macro
////////////////////////////////////////////////////////////////////////////////

// main template
template<bool>
struct StaticAssert;

// specialization
template<>
struct StaticAssert<true>
{};

#define STATIC_ASSERT(exp) \
{ StaticAssert<((exp) != 0)> StaticAssertFailed; }

#ifdef PIL_DEBUG

    #define ASSERT(f) \
        do { \
            if (!f ) { \
                fprintf(stderr, "ERROR (FILE: %s, LINE: %d, FUNC: %s)\n", \
                    __FILE__, __LINE__, __FUNCTION__); \
                exit(1); \
            } \
        } while (0); \


    #define ASSERT2(f, s) \
        do { \
            if (!f ) { \
                fprintf(stderr, "ERROR: %s (FILE: %s, LINE: %d, FUNC: %s)\n", \
                    s, __FILE__, __LINE__, __FUNCTION__); \
                exit(1); \
            } \
        } while (0); \

#else
    #define ASSERT(f)
    #define ASSERT2(f, s)
#endif


////////////////////////////////////////////////////////////////////////////////
/// debug level
////////////////////////////////////////////////////////////////////////////////

#define RTK_DEBUG_LEVEL_ERROR	1
#define RTK_DEBUG_LEVEL_WARN	2
#define RTK_DEBUG_LEVEL_INFO    3
#define RTK_DEBUG_LEVEL_TRACE	4       // default
#define RTK_DEBUG_LEVEL_NORM	5

// debug level functions
void dbg_set_level(int i);
int  dbg_get_level(void);
void dbg_push_level(int i);
int  dbg_pop_level(void);

// log information call back function, regist, and unregist function
typedef std::tr1::function<int (std::string&)> LogInfo_Message_Handle;

int dbg_registMessageHandle(const std::string &handleName, LogInfo_Message_Handle &msgHandle);
int dbg_unregistMessageHandle(const std::string &handleName);


////////////////////////////////////////////////////////////////////////////////
/// usefull debug print functions
////////////////////////////////////////////////////////////////////////////////

// debug print functions
void dbg_printf(int level,
                const char *fname, int line, const char *func,
                const char *szFmtString, ...);

} // end of namespace pi


// level 1: error message
#define MSG_ERROR(...) pi::dbg_printf(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 2: warning message
#define MSG_WARN(...) pi::dbg_printf(2, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 3: information message (default)
#define MSG_INFO(...) pi::dbg_printf(3, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 4: trace message
#define MSG_TRACE(...) pi::dbg_printf(4, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 5: normal message
#define MSG_NORMAL(...) pi::dbg_printf(5, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)



// level 1: error message
#define dbg_pe(...) pi::dbg_printf(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 2: warning message
#define dbg_pw(...) pi::dbg_printf(2, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 3: information message (default)
#define dbg_pi(...) pi::dbg_printf(3, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 4: trace message
#define dbg_pt(...) pi::dbg_printf(4, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 5: normal message
#define dbg_pn(...) pi::dbg_printf(5, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)


#endif // DEBUG_CONFIG_H

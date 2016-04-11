

#ifndef ___OSA_IN_H__
#define ___OSA_IN_H__

#include "base/types/FastList.h"
#include "osa.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
/// magic number
////////////////////////////////////////////////////////////////////////////////
#define OSA_T_MAGIC     0x41670A9B
#define OSA_SEM_MAGIC   0x263567d3
#define	OSA_MU_MAGIC    0xef349aeb
#define OSA_CV_MAGIC    0x42da9be5
#define	OSA_MQ_MAGIC    0xe4b930bc
#define OSA_TM_MAGIC    0xe30981ca


#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif


#ifdef PIL_LINUX
////////////////////////////////////////////////////////////////////////////////
/// Mutex
////////////////////////////////////////////////////////////////////////////////
typedef struct MutexHandle {
    ru32            magic;
    list_t          list;

	pthread_mutex_t	handle;
} MutexHandle_t;

/* Check mutex handle & return mutex descriptor */
static inline OSA_RESULT _osa_mu_checkHandle(OSA_HANDLE hMu, MutexHandle_t **handle)
{
    if( NULL == handle )
        return E_OSA_MU_HANDLE;

    *handle = (MutexHandle_t *) hMu;

    try {
        if( OSA_MU_MAGIC == (*handle)->magic )
            return 0;
        else
            return E_OSA_MU_HANDLE;
    } catch( ... ) {
        return E_OSA_MU_HANDLE;
    }

    return E_OSA_MU_HANDLE;
}


////////////////////////////////////////////////////////////////////////////////
/// thread-specific-data (TSD0
////////////////////////////////////////////////////////////////////////////////
extern pthread_key_t _osa_tid_key;
extern pthread_key_t _osa_data_key;

/* inner used to TSD create/delete */
OSA_RESULT _osa_tsd_create(pthread_key_t *threadVar);
OSA_RESULT _osa_tsd_delete(pthread_key_t *threadVar);

/* inner used to set/get TSD value */
OSA_RESULT _osa_tsd_set(pthread_key_t *threadVar, size_t value);
OSA_RESULT _osa_tsd_get(pthread_key_t *threadVar, size_t *value);

#endif // end of PIL_LINUX

////////////////////////////////////////////////////////////////////////////////
/// debug defines
////////////////////////////////////////////////////////////////////////////////
#if CONFIG_OSA_VERBOSE > 0   /* error message */
    #define d1(x) x
#else
    #define d1(x)
#endif
      
#if CONFIG_OSA_VERBOSE > 1   /* warnning message */
    #define d2(x) x
#else
    #define d2(x)
#endif
      
#if CONFIG_OSA_VERBOSE > 2   /* trace message */
    #define d3(x) x
#else
    #define d3(x)
#endif


// debug print functions
void osa_dbg_printf(int level,
                const char *fname, int line, const char *func,
                const char *szFmtString, ...);

#if CONFIG_OSA_VERBOSE > 0
    // level 1: error message
    #define osa_dbg_pe(...) osa_dbg_printf(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
    #define osa_dbg_pe(...)
#endif

// level 2: warning message
#if CONFIG_OSA_VERBOSE > 1
    #define osa_dbg_pw(...) osa_dbg_printf(2, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
    #define osa_dbg_pw(...)
#endif

// level 3: information message (default)
#if CONFIG_OSA_VERBOSE > 2
    #define osa_dbg_pi(...) osa_dbg_printf(3, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
    #define osa_dbg_pi(...)
#endif

// level 4: trace message
#if CONFIG_OSA_VERBOSE > 3
    #define osa_dbg_pt(...) osa_dbg_printf(4, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
    #define osa_dbg_pt(...)
#endif

// level 5: normal message
#if CONFIG_OSA_VERBOSE > 4
    #define osa_dbg_pn(...) osa_dbg_printf(5, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
    #define osa_dbg_pn(...)
#endif


#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

} // end of namespace pi

#endif /* end of ___OSA_IN_H__ */


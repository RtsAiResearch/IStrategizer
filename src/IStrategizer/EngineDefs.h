#ifndef ENGINEDEFS_H
#define ENGINEDEFS_H

#include <process.h>       // For _beginthreadex
#include "Logger.h"

#ifndef CBRB
#define CBRB(x)                                               \
    {                                                       \
    if(!(x))                                         \
    {                                                       \
    LogError("'%s' failed", #x);                \
    return false;                                        \
    }                                                       \
    }
#else
#define CBRB(x) (x)
#endif

#ifndef CBR
#define CBR(x)                                               \
    {                                                       \
    if(!(x))                                         \
    {                                                       \
    LogError("'%s' failed", #x);                \
    return;                                        \
    }                                                       \
    }
#else
#define CBR(x) (x)
#endif

#define DISALLOW_COPY_AND_ASSIGN(C) \
    C(const C&); C& operator = (const C&);

// This macro function calls the C runtime's _beginthreadex function. 
// The C runtime library doesn't want to have any reliance on Windows' data 
// types such as HANDLE. This means that a Windows programmer needs to cast
// values when using _beginthreadex. Since this is terribly inconvenient, 
// I created this macro to perform the casting.
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
    pvParam, dwCreateFlags, pdwThreadId)                 \
    ((HANDLE)_beginthreadex(                          \
    (void *)        (psa),                         \
    (unsigned)      (cbStackSize),                 \
    (PTHREAD_START) (pfnStartAddr),                \
    (void *)        (pvParam),                     \
    (unsigned)      (dwCreateFlags),               \
    (unsigned *)    (pdwThreadId)))


#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define iNEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define iNEW new
#endif

#undef SAFE_DELETE
#define SAFE_DELETE(X)  { if ((X) != nullptr) { delete (X); (X)=nullptr; } }
#undef SAFE_CLOSE
#define SAFE_CLOSE(X)   { if (X) { CloseHandle(X); X = nullptr; } }

#endif // ENGINEDEFS_H

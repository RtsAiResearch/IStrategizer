#include "EngineObject.h"
#include "EngineDefs.h"
#include <Windows.h>

using namespace IStrategizer;

HANDLE g_hHeap = NULL;

void* EngineObject::Alloc(std::size_t sz)
{
    if (NULL == g_hHeap)
    {
        g_hHeap = HeapCreate(HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, 0 , 0);
        _ASSERTE(NULL != g_hHeap);
    }

    return HeapAlloc(g_hHeap, 0, sz);
}
//////////////////////////////////////////////////////////////////////////
void EngineObject::Free(void* pObj)
{
    (void)HeapFree(g_hHeap, 0, pObj);
}
//////////////////////////////////////////////////////////////////////////
void EngineObject::FreeMemoryPool()
{
    if (g_hHeap)
    {
        HeapDestroy(g_hHeap);
        g_hHeap = NULL;

        LogInfo("EngineObject memory pool heap destroyed");
    }
}
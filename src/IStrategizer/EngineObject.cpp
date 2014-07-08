#include "EngineObject.h"
#include "EngineDefs.h"
#include <Windows.h>
#include <unordered_set>
#include <unordered_map>

using namespace IStrategizer;
using namespace std;

HANDLE g_hHeap = NULL;
unordered_set<EngineObject*>* g_pAliveObjects = nullptr;
size_t g_AliveObjectsUsedMem = 0;

void* EngineObject::Alloc(std::size_t sz)
{
    if (NULL == g_hHeap)
    {
        g_hHeap = HeapCreate(HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, 0 , 0);
        _ASSERTE(NULL != g_hHeap);

        g_pAliveObjects = new unordered_set<EngineObject*>;
    }

    void* pMem = HeapAlloc(g_hHeap, 0, sz);
    g_AliveObjectsUsedMem += HeapSize(g_hHeap, 0, pMem);

    return pMem;
}
//////////////////////////////////////////////////////////////////////////
void EngineObject::Free(void* pMem)
{
    g_AliveObjectsUsedMem -= HeapSize(g_hHeap, 0, pMem);
    (void)HeapFree(g_hHeap, 0, pMem);
}
//////////////////////////////////////////////////////////////////////////
size_t EngineObject::AliveObjectsCount() { return g_pAliveObjects->size(); }
//////////////////////////////////////////////////////////////////////////
size_t EngineObject::AliveObjectsMemoryUsage() { return g_AliveObjectsUsedMem; }
//////////////////////////////////////////////////////////////////////////
void EngineObject::FreeMemoryPool()
{
    if (g_hHeap)
    {
        LogInfo("Freeing EngineObject memory pool");

        if (!g_pAliveObjects->empty())
        {
            LogInfo("Found %d alive EngineObject, destroying them...", g_pAliveObjects->size());

            while (!g_pAliveObjects->empty())
            {
                auto pCurrObj = *g_pAliveObjects->begin();
                g_pAliveObjects->erase(pCurrObj);
                // The delete should call the destructor of EngineObject which
                // in turn will remove the object from the alive objects
                delete pCurrObj;
            }

            _ASSERTE(g_pAliveObjects->empty());
        }

        (void)HeapDestroy(g_hHeap);
        g_hHeap = NULL;

        SAFE_DELETE(g_pAliveObjects);

        LogInfo("EngineObject memory pool heap destroyed");
    }
}
//////////////////////////////////////////////////////////////////////////
void EngineObject::DumpAliveObjects()
{
    if (g_pAliveObjects->empty())
        LogInfo("All EngineObject instances are dead, nothing to dump");
    else
    {
        LogInfo("Dumping %d alive EngineObject", g_pAliveObjects->size());
        
        size_t count = 0;
        for (auto pObj : *g_pAliveObjects)
        {
            LogInfo("Alive EngineObject[%d]@0x%x: %s", count, (void*)pObj, pObj->ToString().c_str());
            ++count;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
EngineObject::EngineObject()
{
    g_pAliveObjects->insert(this);
    LogInfo("EngineObject@0x%x created", (void*)this);
}
//////////////////////////////////////////////////////////////////////////
EngineObject::~EngineObject()
{
    g_pAliveObjects->erase(this);
    LogInfo("EngineObject@0x%x destroyed", (void*)this);
}
//////////////////////////////////////////////////////////////////////////
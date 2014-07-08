#include "EngineObject.h"
#include "EngineDefs.h"
#include <Windows.h>
#include <unordered_set>
#include <unordered_map>

using namespace IStrategizer;
using namespace std;

HANDLE g_hHeap = NULL;
unordered_set<EngineObject*> g_AliveObjects;
size_t g_AliveObjectsUsedMem = 0;

void* EngineObject::Alloc(std::size_t sz)
{
    if (NULL == g_hHeap)
    {
        g_hHeap = HeapCreate(HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, 0 , 0);
        _ASSERTE(NULL != g_hHeap);
    }

    g_AliveObjectsUsedMem += sz;
    return HeapAlloc(g_hHeap, 0, sz);;
}
//////////////////////////////////////////////////////////////////////////
void EngineObject::Free(void* pObj)
{
    g_AliveObjectsUsedMem -= HeapSize(g_hHeap, 0, pObj);
    (void)HeapFree(g_hHeap, 0, pObj);
}
//////////////////////////////////////////////////////////////////////////
size_t EngineObject::AliveObjectsCount() { return g_AliveObjects.size(); }
//////////////////////////////////////////////////////////////////////////
size_t EngineObject::AliveObjectsMemoryUsage() { return g_AliveObjectsUsedMem; }
//////////////////////////////////////////////////////////////////////////
void EngineObject::FreeMemoryPool()
{
    if (g_hHeap)
    {
        LogInfo("Freeing EngineObject memory pool");

        if (!g_AliveObjects.empty())
        {
            LogInfo("Found %d alive EngineObject, destroying them...", g_AliveObjects.size());

            while (!g_AliveObjects.empty())
            {
                auto pCurrObj = *g_AliveObjects.begin();
                g_AliveObjects.erase(pCurrObj);
                // The delete should call the destructor of EngineObject which
                // in turn will remove the object from the alive objects
                delete pCurrObj;
            }

            _ASSERTE(g_AliveObjects.empty());
        }

        (void)HeapDestroy(g_hHeap);
        g_hHeap = NULL;
        LogInfo("EngineObject memory pool heap destroyed");
    }
}
//////////////////////////////////////////////////////////////////////////
void EngineObject::DumpAliveObjects()
{
    if (g_AliveObjects.empty())
        LogInfo("All EngineObject instances are dead, nothing to dump");
    else
    {
        LogInfo("Dumping %d alive EngineObject", g_AliveObjects.size());
        
        size_t count = 0;
        for (auto pObj : g_AliveObjects)
        {
            LogInfo("Alive EngineObject[%d]@0x%x: %s", count, (void*)pObj, pObj->ToString().c_str());
            ++count;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
EngineObject::EngineObject()
{
    g_AliveObjects.insert(this);
}
//////////////////////////////////////////////////////////////////////////
EngineObject::~EngineObject()
{
    g_AliveObjects.erase(this);
}
//////////////////////////////////////////////////////////////////////////

#ifndef ENGINEOBJECT_H
#define ENGINEOBJECT_H

#include <string>
#include "IMessagePumpObserver.h"
#include "ISerializable.h"

namespace IStrategizer
{
    class EngineObject : public Serialization::ISerializable, public IMessagePumpObserver
    {
		OBJECT_SERIALIZABLE(EngineObject);
    public:
        EngineObject();
        ~EngineObject();
        virtual std::string ToString(bool minimal = false) const { return typeid(*this).name(); }
        void NotifyMessegeSent(Message* p_pMessage) {}

        static void* operator new(std::size_t sz) { return Alloc(sz); }
        static void* operator new[](std::size_t sz) { return Alloc(sz); }
        static void operator delete(void* pObj) { Free(pObj); }
        static void operator delete[](void* pObj, std::size_t sz) { Free(pObj); }
        static void FreeMemoryPool();
        static void DumpAliveObjects();
        static size_t AliveObjectsCount();
        static size_t AliveObjectsMemoryUsage();

    private:
        static void* Alloc(std::size_t sz);
        static void Free(void* pObj);
    };
}

#endif // ENGINEOBJECT_H
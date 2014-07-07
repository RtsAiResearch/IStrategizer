#ifndef ENGINEOBJECT_H
#define ENGINEOBJECT_H

#include <string>

namespace IStrategizer
{
    class EngineObject
    {
    public:
        ~EngineObject() {}
        virtual std::string ToString(bool minimal = false) const = 0;

        static void* operator new(std::size_t sz) { return Alloc(sz); }
        static void* operator new[](std::size_t sz) { return Alloc(sz); }
        static void operator delete(void* pObj) { Free(pObj); }
        static void operator delete[](void* pObj, std::size_t sz) { Free(pObj); }

        static void* Alloc(std::size_t sz);
        static void Free(void* pObj);
        static void FreeMemoryPool();
    };
}

#endif // ENGINEOBJECT_H
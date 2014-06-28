#ifndef CONTAINER_H
#define CONTAINER_H

#ifndef SERIALIZABLE_H
    #include "ISerializable.h"
#endif

namespace Serialization
{
    class Container : public ISerializable
    {
    public:
        enum Cookie
        {
            DefaultCtor     = 0xfeedbeef,
        };

        Container() : m_cookie(DefaultCtor), m_objID(++sm_lastSerializableObjID) {}

        virtual int         ContainerCount()    = 0;
        virtual void        Clear()             = 0;
        virtual void        AddTemp()           = 0;
        virtual char*       GetTemp()           = 0;

    private:
        Cookie m_cookie;
        size_t m_objID;
    };
}
#endif // CONTAINER_H

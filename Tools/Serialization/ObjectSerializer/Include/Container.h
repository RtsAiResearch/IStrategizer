#ifndef CONTAINER_H
#define CONTAINER_H

#ifndef SERIALIZABLE_H
	#include "Serializable.h"
#endif

namespace Serialization
{
    class Container : public Serializable
    {
    public:
        virtual int         ContainerCount()    = 0;
        virtual void        Clear()             = 0;
        virtual void        AddTemp()           = 0;
        virtual char*       GetTemp()           = 0;
    };
}
#endif // CONTAINER_H

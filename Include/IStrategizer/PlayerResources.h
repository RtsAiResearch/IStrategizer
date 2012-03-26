#ifndef PLAYERRESOURCES_H
#define PLAYERRESOURCES_H

#ifndef WORLDCOMPONENT_H
#include "WorldComponent.h"
#endif
#ifndef WORLDRESOURCES_H
#include "WorldResources.h"
#endif

namespace IStrategizer
{
    class PlayerResources
    {
    public:
		virtual ~PlayerResources() {}
		virtual bool HasEnough(const WorldResources* p_resources);
		virtual int Supply() = 0;
		virtual int Secondary() = 0;
		virtual int Primary() = 0;
    };
}


#endif // PLAYERRESOURCES_H
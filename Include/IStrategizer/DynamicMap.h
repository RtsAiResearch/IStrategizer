#ifndef DYNAMICMAP_H
#define DYNAMICMAP_H

#ifndef DYNAMICCOMPONENT_H
#include "DynamicComponent.h"
#endif

#ifndef MAPEX_H
#include "MapEx.h"
#endif

namespace IStrategizer
{
    using namespace DataStructure;
    template<class TKey, class TValue>
    class DynamicMap : public MapEx<TKey, TValue>, public DynamicComponent
    {
    };
}


#endif // DYNAMICMAP_H
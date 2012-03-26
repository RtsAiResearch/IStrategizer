#ifndef DYNAMICVECTOR_H
#define DYNAMICVECTOR_H

#ifndef DYNAMICCOMPONENT_H
#include "DynamicComponent.h"
#endif

#include <vector>

namespace IStrategizer
{
    template<class T>
    class DynamicVector : public std::vector<T>, public DynamicComponent
    {
    };
}

#endif // DYNAMICVECTOR_H
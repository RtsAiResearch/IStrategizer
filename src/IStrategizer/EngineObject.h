#ifndef ENGINEOBJECT_H
#define ENGINEOBJECT_H

#include <string>

namespace IStrategizer
{
    class EngineObject
    {
    public:
        virtual std::string ToString(bool minimal = false) const = 0;
    };
}

#endif // ENGINEOBJECT_H
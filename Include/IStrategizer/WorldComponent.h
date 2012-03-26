#ifndef WORLDCOMPONENT_H
#define WORLDCOMPONENT_H

#ifndef DYNAMICCOMPONENT_H
#include "DynamicComponent.h"
#endif

namespace IStrategizer
{
    class WorldComponent : public DynamicComponent
    {
    protected:
        virtual void UpdateAux() = 0;

    public:
                WorldComponent(unsigned p_invalidationInterval) : DynamicComponent(p_invalidationInterval) {}
        void    Update();
    };
}

#endif // WORLDCOMPONENT_H
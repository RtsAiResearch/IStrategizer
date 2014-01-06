#ifndef WORLDCOMPONENT_H
#define WORLDCOMPONENT_H

#ifndef DYNAMICCOMPONENT_H
#include "DynamicComponent.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif

namespace IStrategizer
{
    class WorldComponent : public DynamicComponent
    {
    protected:
        virtual void UpdateAux(RtsGame* pRtsGame, const WorldClock& p_clock) = 0;

    public:
        WorldComponent(unsigned p_invalidationInterval) : DynamicComponent(p_invalidationInterval) {}
        void Update(const WorldClock& p_clock);
    };
}

#endif // WORLDCOMPONENT_H

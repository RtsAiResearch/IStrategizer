#ifndef FINISHEDFSMSTATE_H
#define FINISHEDFSMSTATE_H

#include "FSMState.h"
#include "EngineData.h"

namespace IStrategizer
{
    template<class TController, class TControllerTraits = ControllerTraits<TController>>
    class FinishedFSMState : public FSMState<TController, ControllerTraits<TController>>
    {
    public:
        FinishedFSMState(TController controller) : FSMState(Finished, controller) {}
    };
}

#endif // FINISHEDFSMSTATE_H
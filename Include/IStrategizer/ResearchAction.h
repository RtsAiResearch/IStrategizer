///> [Serializable]
#ifndef RESEARCHACTION_H
#define RESEARCHACTION_H

#include "Action.h"
#ifndef AND_H
#include "And.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

namespace IStrategizer
{
    class CellFeature;

    ///> class=ResearchAction
    ///> parent=Action
    class ResearchAction : public Action
    {
        OBJECT_SERIALIZABLE(ResearchAction);

    public:
        ResearchAction();
        ResearchAction(const PlanStepParameters& p_parameters);
        bool AliveConditionsSatisfied(RtsGame& p_RtsGame);
        bool SuccessConditionsSatisfied(RtsGame& p_RtsGame);

    protected:
        bool ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void InitializePostConditions();
        void InitializePreConditions(RtsGame& p_RtsGame);

    private:
        TID _researcherId;
    };
}

#endif // RESEARCHACTION_H

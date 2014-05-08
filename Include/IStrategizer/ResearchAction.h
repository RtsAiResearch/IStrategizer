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
        bool AliveConditionsSatisfied(RtsGame& game);
        bool SuccessConditionsSatisfied(RtsGame& game);

    protected:
        bool ExecuteAux(RtsGame& game, const WorldClock& p_clock);
        void InitializePostConditions();
        void InitializePreConditions();

    private:
        TID m_researcherId;
        EntityClassType m_researcherType;
    };
}

#endif // RESEARCHACTION_H

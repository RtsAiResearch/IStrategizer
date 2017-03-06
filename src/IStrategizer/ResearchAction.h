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
        OBJECT_SERIALIZABLE_P(ResearchAction, Action);

    public:
        ResearchAction();
        ResearchAction(const PlanStepParameters& p_parameters);
        bool AliveConditionsSatisfied();
        bool SuccessConditionsSatisfied(RtsGame& game);
        bool Equals(PlanStepEx* p_planStep);
        void FreeResources();

    protected:
        bool Execute();
        void InitializePostConditions();
        void InitializePreConditions();

    private:
        TID m_researcherId;
        EntityClassType m_researcherType;
    };
}

#endif // RESEARCHACTION_H

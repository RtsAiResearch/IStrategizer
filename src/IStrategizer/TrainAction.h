///> [Serializable]
#ifndef TRAINACTION_H
#define TRAINACTION_H

#include "Action.h"
#ifndef AND_H
#include "And.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

#ifndef WORLDRESOURCES_H
#include "WorldResources.h"
#endif

namespace IStrategizer
{
    class CellFeature;

    ///> class=TrainAction
    ///> parent=Action
    class TrainAction : public Action
    {
        OBJECT_SERIALIZABLE_P(TrainAction, Action);

    public:
        TrainAction();
        TrainAction(const PlanStepParameters& params);
        bool AliveConditionsSatisfied();
        bool SuccessConditionsSatisfied(RtsGame& game);
        bool Equals(PlanStepEx* p_planStep);

    protected:
        bool Execute();
        void HandleMessage(Message* pMsg, bool& consumed);
        void InitializePostConditions();
        void InitializePreConditions();
        void FreeResources();

    private:
        TID m_trainerId;
        TID m_traineeId;
        bool m_trainStarted;
        //WorldResources m_requiredResources;
        EntityClassType m_trainerType;
    };
}

#endif // TRAINACTION_H

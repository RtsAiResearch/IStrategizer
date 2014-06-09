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
        OBJECT_SERIALIZABLE(TrainAction);

    public:
        TrainAction();
        TrainAction(const PlanStepParameters& params);
        bool AliveConditionsSatisfied(RtsGame& game);
        bool SuccessConditionsSatisfied(RtsGame& game);
        bool Equals(PlanStepEx* p_planStep);
        unsigned Hash() const { return StepTypeId() + Parameter(PARAM_EntityClassId); }

    protected:
        void OnSucccess(RtsGame& game, const WorldClock& clock);
        void OnFailure(RtsGame& game, const WorldClock& clock);
        bool ExecuteAux(RtsGame& game, const WorldClock& clock);
        void HandleMessage(RtsGame& game, Message* pMsg, bool& consumed);
        void InitializePostConditions();
        void InitializePreConditions();
        void FreeResources(RtsGame& game);

    private:
        TID m_trainerId;
        TID m_traineeId;
        GameEntity* m_pTrainee;
        bool m_trainStarted;
        WorldResources m_requiredResources;
        EntityClassType m_trainerType;
    };
}

#endif // TRAINACTION_H

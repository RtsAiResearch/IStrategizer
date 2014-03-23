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
        TrainAction(const PlanStepParameters& p_parameters);
        bool AliveConditionsSatisfied(RtsGame& pRtsGame);
        bool SuccessConditionsSatisfied(RtsGame& pRtsGame);

    protected:
        void OnSucccess(RtsGame& pRtsGame, const WorldClock& p_clock);
        void OnFailure(RtsGame& pRtsGame, const WorldClock& p_clock);
        bool ExecuteAux(RtsGame& pRtsGame, const WorldClock& p_clock);
        void HandleMessage(RtsGame& pRtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions();

    private:
        TID _trainerId;
        TID _traineeId;
        GameEntity* m_pTrainee;
        bool _trainStarted;
    };
}

#endif // TRAINACTION_H

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
        bool AliveConditionsSatisfied(RtsGame& p_RtsGame);
        bool SuccessConditionsSatisfied(RtsGame& p_RtsGame);

    protected:
        bool ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void HandleMessage(RtsGame& p_RtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions(RtsGame& p_RtsGame);

    private:
        TID _trainerId;
        TID _traineeId;
        bool _trainStarted;
    };
}

#endif // TRAINACTION_H

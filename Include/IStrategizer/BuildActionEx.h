///> [Serializable]
#ifndef BUILDACTIONEX_H
#define BUILDACTIONEX_H

#include "Action.h"
#include "And.h"
#include "Vector2.h"
#include "MapArea.h"
#include "RtsGame.h"

namespace IStrategizer
{
    class CellFeature;

    ///> class=BuildActionEx
    ///> parent=Action
    class BuildActionEx : public Action
    {
        OBJECT_SERIALIZABLE(BuildActionEx);

    public:
        BuildActionEx();
        BuildActionEx(const PlanStepParameters& p_parameters);
        bool AliveConditionsSatisfied(RtsGame& p_RtsGame);
        bool SuccessConditionsSatisfied(RtsGame& p_RtsGame);

    protected:
        void OnSucccess(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void OnFailure(RtsGame& p_RtsGame, const WorldClock& p_clock);
        bool ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void HandleMessage(RtsGame& p_RtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions(RtsGame& p_RtsGame);

    private:
        TID _buildingId;
        TID _builderId;
        IStrategizer::MapArea _buildArea;
        bool _buildStarted;
        bool _buildIssued;
    };
}

#endif // BUILDACTIONEX_H

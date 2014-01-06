///> [Serializable]
#ifndef BUILDACTIONEX_H
#define BUILDACTIONEX_H

#include "Action.h"
#ifndef AND_H
#include "And.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif
#ifndef MAPAREA_H
#include "MapArea.h"
#endif

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
        bool AliveConditionsSatisfied(RtsGame* pRtsGame);
        bool SuccessConditionsSatisfied(RtsGame* pRtsGame);

    protected:
        void OnSucccess(RtsGame* pRtsGame, const WorldClock& p_clock);
        void OnFailure(RtsGame* pRtsGame, const WorldClock& p_clock);
        bool ExecuteAux(RtsGame* pRtsGame, const WorldClock& p_clock);
        void HandleMessage(RtsGame *pRtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions();

    private:
        TID _buildingId;
        TID _builderId;
        IStrategizer::MapArea _buildArea;
        bool _buildStarted;
        bool _buildIssued;
    };
}

#endif // BUILDACTIONEX_H

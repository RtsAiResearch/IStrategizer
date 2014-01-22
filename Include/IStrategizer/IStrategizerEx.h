#ifndef ISTRATEGIZEREX_H
#define ISTRATEGIZEREX_H

#include <vector>
#include "MetaData.h"
#include "MessagePumpObserver.h"
#include "WorldClock.h"
#include "RtsGame.h"

namespace IStrategizer
{
    class MessagePump;
    class GameTrace;
    class CaseBaseEx;
    class LearningFromHumanDemonstration;
    class GameAPI;
    class OnlineCaseBasedPlannerEx;
    class RtsGame;

    struct IStrategizerParam
    {
        unsigned GrndCtrlIMUpdateInterval;
        unsigned OccupanceIMUpdateInterval;
        int BuildingDataIMCellSize;
        int GrndCtrlIMCellSize;
        PhaseType Phase;
    };

    class IStrategizerEx : public MessagePumpObserver
    {
    private:
        OnlineCaseBasedPlannerEx* _planner;
        LearningFromHumanDemonstration* _caseLearning;
        PlayerType _self;
        PlayerType _enemy;
        IStrategizerParam _param;
        bool _isFirstUpdate;
        WorldClock _clock;
        RtsGame& m_RtsGame;

    public:
        IStrategizerEx(const IStrategizerParam &p_param, RtsGame& p_RtsGame);
        void    Update(unsigned p_gameCycle);
        void    StartOfflineLearning();
        const OnlineCaseBasedPlannerEx* Planner() const { return _planner; }
        OnlineCaseBasedPlannerEx* Planner() { return _planner; }
        void    NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_message);
        const WorldClock& Clock() const { return _clock; }
        ~IStrategizerEx();
    };
}

#endif    // ISTRATEGIZEREX_H

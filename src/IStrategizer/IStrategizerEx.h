#ifndef ISTRATEGIZEREX_H
#define ISTRATEGIZEREX_H

#include <vector>
#include "MetaData.h"
#include "MessagePumpObserver.h"
#include "WorldClock.h"
#include "AttackManager.h"

namespace IStrategizer
{
    class MessagePump;
    class GameTrace;
    class CaseBaseEx;
    class LearningFromHumanDemonstration;
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
        OnlineCaseBasedPlannerEx* m_pPlanner;
        LearningFromHumanDemonstration* m_pCaseLearning;
        IStrategizerParam m_param;
        bool m_isFirstUpdate;
        WorldClock m_clock;
        AttackManager m_attackManager;

    public:
        IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame);
        void Update(unsigned gameCycle);
        const OnlineCaseBasedPlannerEx* Planner() const { return m_pPlanner; }
        OnlineCaseBasedPlannerEx* Planner() { return m_pPlanner; }
        void NotifyMessegeSent(Message* pMsg);
        const WorldClock& Clock() const { return m_clock; }
        bool Init();
        ~IStrategizerEx();
    };
}

#endif // ISTRATEGIZEREX_H
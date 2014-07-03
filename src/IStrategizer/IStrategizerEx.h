#ifndef ISTRATEGIZEREX_H
#define ISTRATEGIZEREX_H

#include "MetaData.h"
#include "MessagePumpObserver.h"
#include "WorldClock.h"
#include "AttackManager.h"
#include <vector>

namespace IStrategizer
{
    class MessagePump;
    class GameTrace;
    class CaseBaseEx;
    class LearningFromHumanDemonstration;
    class OnlineCaseBasedPlannerEx;
    class RtsGame;
    class BotStatistics;

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
    public:
        IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame);
        void Update(unsigned gameCycle);
        void NotifyMessegeSent(Message* pMsg);
        bool Init();
        const OnlineCaseBasedPlannerEx* Planner() const { return m_pPlanner; }
        OnlineCaseBasedPlannerEx* Planner() { return m_pPlanner; }
        const WorldClock& Clock() const { return m_clock; }
        ~IStrategizerEx();

    private:
        void DefineArmyTrainOrder();
        int GetTrainOrderInx();

        bool m_isFirstUpdate;
        unsigned m_armyTrainOrderInx;
        OnlineCaseBasedPlannerEx* m_pPlanner;
        LearningFromHumanDemonstration* m_pCaseLearning;
        IStrategizerParam m_param;
        WorldClock m_clock;
        AttackManager m_attackManager;
        std::vector<PlanStepParameters> m_armyTrainOrder;
        BotStatistics* m_pStatistics;
    };
}

#endif // ISTRATEGIZEREX_H
#ifndef ISTRATEGIZEREX_H
#define ISTRATEGIZEREX_H

#include "MetaData.h"
#include "IMessagePumpObserver.h"
#include "WorldClock.h"
#include "AttackManager.h"
#include "ResourceManager.h"
#include <vector>
#include <memory>

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
        StrategySelector* pStrategySelector;
        std::string map;
    };

    class IStrategizerEx : public EngineObject
    {
    public:
        IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame);
        void Update(unsigned gameCycle);
        void NotifyMessegeSent(Message* pMsg);
        bool Init();
        const OnlineCaseBasedPlannerEx* Planner() const { return &*m_pPlanner; }
        OnlineCaseBasedPlannerEx* Planner() { return &*m_pPlanner; }
        const WorldClock& Clock() const { return m_clock; }
        ~IStrategizerEx();

    private:
        PlanStepParameters StartTrainingArmy();
        
        bool m_isFirstUpdate;
        std::shared_ptr<OnlineCaseBasedPlannerEx> m_pPlanner;
        std::shared_ptr<LearningFromHumanDemonstration> m_pCaseLearning;
        IStrategizerParam m_param;
        WorldClock m_clock;
        AttackManager m_attackManager;
        StrategySelector* m_pStrategySelector;
        std::shared_ptr<BotStatistics> m_pStatistics;
		ResourceManager m_resourceManager;
    };
}

#endif // ISTRATEGIZEREX_H
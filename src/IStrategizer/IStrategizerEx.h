#ifndef ISTRATEGIZEREX_H
#define ISTRATEGIZEREX_H

#include "MetaData.h"
#include "IMessagePumpObserver.h"
#include "WorldClock.h"
#include "CombatManager.h"
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
        StrategySelectorPtr pStrategySelector;
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
        ~IStrategizerEx();

    private:
        void SelectNextProductionGoal();

        bool m_isFirstUpdate;
        std::shared_ptr<OnlineCaseBasedPlannerEx> m_pPlanner;
        std::shared_ptr<LearningFromHumanDemonstration> m_pCaseLearning;
        IStrategizerParam m_param;
        CombatManager m_combatManager;
		ResourceManager m_resourceManager;
        std::shared_ptr<BotStatistics> m_pStatistics;
    };
}

#endif // ISTRATEGIZEREX_H
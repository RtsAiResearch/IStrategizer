#ifndef ISTRATEGIZEREX_H
#define ISTRATEGIZEREX_H

#include "MetaData.h"
#include "IMessagePumpObserver.h"
#include "WorldClock.h"
#include "CombatManager.h"
#include "WorkersManager.h"
#include "ScoutManager.h"
#include "EngineDefs.h"
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
        int OccupanceIMCellSize;
        int GrndCtrlIMCellSize;
        PhaseType Phase;
        StrategySelectorPtr Consultant;
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
        WorkersManager& WorkersMgr() { return m_workersMgr; }

        ~IStrategizerEx();

        static std::string sm_WorkingDir;

    private:
        DISALLOW_COPY_AND_ASSIGN(IStrategizerEx);

        const unsigned ScoutStartFrame = 2000;
        void SelectNextProductionGoal();

        bool m_isFirstUpdate;
        std::shared_ptr<LearningFromHumanDemonstration> m_pCaseLearning;
        IStrategizerParam m_param;
        std::shared_ptr<BotStatistics> m_pStatistics;

        std::shared_ptr<OnlineCaseBasedPlannerEx> m_pPlanner;
        CombatManager m_combatMgr;
        ScoutManager m_scoutMgr;
        WorkersManager m_workersMgr;
    };
}

extern IStrategizer::IStrategizerEx* g_Engine;

#endif // ISTRATEGIZEREX_H
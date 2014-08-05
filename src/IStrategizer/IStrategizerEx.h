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

extern std::string ENGINE_IO_READ_DIR;
extern std::string ENGINE_IO_WRITE_DIR;

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
        static const int BordersRadius = 1024;
        static const int ReviseSituationInterval = 16;

        IStrategizerEx(const IStrategizerParam &param, RtsGame* pGame);
        void Update(unsigned gameCycle);
        void NotifyMessegeSent(Message* pMsg);
        bool Init();
        const OnlineCaseBasedPlannerEx* Planner() const { return &*m_pPlanner; }
        OnlineCaseBasedPlannerEx* Planner() { return &*m_pPlanner; }
        WorkersManager& WorkersMgr() { return m_workersMgr; }
        ScoutManager& ScoutMgr() { return m_scoutMgr; }
        Vector2F BaseHeadDirection() const { return m_baseFaceDir; }
        SituationType Situation() const { return m_situation; }
        void ReviseSituation();

        void DebugDraw();

        ~IStrategizerEx();

    private:
        DISALLOW_COPY_AND_ASSIGN(IStrategizerEx);

        const unsigned ScoutStartFrame = 2000;
        void SelectNextStrategyGoal();

        bool m_isFirstUpdate;
        std::shared_ptr<LearningFromHumanDemonstration> m_pCaseLearning;
        IStrategizerParam m_param;
        std::shared_ptr<BotStatistics> m_pStatistics;

        std::shared_ptr<OnlineCaseBasedPlannerEx> m_pPlanner;
        CombatManager m_combatMgr;
        ScoutManager m_scoutMgr;
        WorkersManager m_workersMgr;
        Vector2F m_baseFaceDir;
        SituationType m_situation;
        Circle2 m_borders;
    };
}

extern IStrategizer::IStrategizerEx* g_Engine;

#endif // ISTRATEGIZEREX_H
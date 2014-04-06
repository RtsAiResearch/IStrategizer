#ifndef TRACEEX_H
#define TRACEEX_H

#include <vector>
#include <string>
#include "EngineData.h"
#include "GameStateEx.h"

namespace IStrategizer
{
    class GameStateEx;

    class GameTrace
    {
    private:
        unsigned       m_gameCycle;
        ActionType     m_action;
        PlanStepParameters   m_actionParams;
        GameStateEx     m_gameState;
        PlayerType     m_self;

    public:
        typedef std::vector<GameTrace> List;

        GameTrace() { }

        GameTrace(
            unsigned p_gameCycle,
            ActionType p_action,
            const PlanStepParameters& p_actionParams, 
            const GameStateEx& p_pGameState,
            PlayerType p_self)
            : m_gameCycle(p_gameCycle),
            m_action(p_action),
            m_actionParams(p_actionParams),
            m_gameState(p_pGameState),
            m_self(p_self)
    {}

        unsigned GameCycle() const { return m_gameCycle; }
        ActionType Action() const { return m_action; }
        const PlanStepParameters& ActionParams() const { return m_actionParams; }
        const GameStateEx& GameState() const { return m_gameState; }
        PlayerType Self() const { return m_self; }
    std::string ToString() const;
    };
}

#endif // TRACEEX_H

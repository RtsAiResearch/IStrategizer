#ifndef TRACEEX_H
#define TRACEEX_H

#include "EngineData.h"
#include <vector>
#include "GameStateEx.h"

namespace IStrategizer
{
	class GameStateEx;

	// N X 1 matrix of booleans, where N = number supported goals, 
	// where GoalMatrix[i] indicates whether the goal was satisfied or not
	typedef std::vector<bool> GoalMatrix;

	class GameTrace
	{
	private:
		unsigned				m_gameCycle;
		ActionType				m_action;
		PlanStepParameters	    m_actionParams;
		GameStateEx				m_gameState;
		GoalMatrix				m_satisfiedGoals;
		PlayerType				m_self;
		PlayerType				m_enemy;

	public:
		typedef std::vector<GameTrace*>	List;

		GameTrace(
			unsigned p_gameCycle,
			ActionType p_action,
			const PlanStepParameters& p_actionParams, 
			const GameStateEx& p_pGameState,
			PlayerType p_self,
			PlayerType p_enemy)
			: m_gameCycle(p_gameCycle),
			m_action(p_action),
			m_actionParams(p_actionParams),
			m_gameState(),
			m_self(p_self),
			m_enemy(p_enemy) {}

		unsigned				GameCycle() const { return m_gameCycle; }
		ActionType						Action() const { return m_action; }
		const PlanStepParameters&		ActionParams() const { return m_actionParams; }
		GoalMatrix				GoalSatisfaction() const { return m_satisfiedGoals; }
		const GameStateEx&		GameState() const { return m_gameState; }
		PlayerType				Self() const { return m_self; }
		PlayerType				Enemy() const { return m_enemy; }
	};
}

#endif	// TRACEEX_H
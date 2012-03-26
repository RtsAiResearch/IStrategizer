#ifndef TRACEEX_H
#define TRACEEX_H

#include "EngineAssist.h"
#include "EngineData.h"

class GameStateEx;

class TraceEx
{
private:
	unsigned long			_gameCycle;
	int						_abstractActionId;
	PlanStepParameters	    _abstractParametersId;
	GameStateEx*			_gameState;
	GoalSatisfactionEx		_goalSatisfaction;
	PlayerType				_player;
	PlayerType				_enemy;
	
public:
	inline 							TraceEx() {}
    TraceEx(unsigned long p_gameCycle, int p_abstractActionId, const PlanStepParameters& p_abstractParametersId, const GoalSatisfactionEx& p_goalSatisfaction, GameStateEx* p_gameState, PlayerType p_player, PlayerType p_enemy);
    TraceEx(unsigned long p_gameCycle, int p_abstractActionId, const PlanStepParameters& p_abstractParametersId, GameStateEx* p_gameState, PlayerType p_player, PlayerType p_enemy);
	inline unsigned long			GameCycle() const { return _gameCycle; }
	inline void						GameCycle(unsigned long val) { _gameCycle = val; }
	inline int						AbstractActionId() const { return _abstractActionId; }
	inline void						AbstractActionId(int val) { _abstractActionId = val; }
	inline PlanStepParameters	AbstractParametersId() const { return _abstractParametersId; }
	inline void						AbstractParametersId(PlanStepParameters val) { _abstractParametersId = val; }
	inline GoalSatisfactionEx		GoalSatisfaction() const { return _goalSatisfaction; }
	inline void						GoalSatisfaction(GoalSatisfactionEx val) { _goalSatisfaction = val; }
	inline GameStateEx*				GameState() const { return _gameState; }
	inline void						GameState(GameStateEx* val) { _gameState = val; }
	inline PlayerType				Player() const { return _player; }
	inline void						Player(PlayerType val) { _player = val; }
	inline PlayerType				Enemy() const { return _enemy; }
	inline void						Enemy(PlayerType val) { _enemy = val; }
};
#endif	// TRACEEX_H
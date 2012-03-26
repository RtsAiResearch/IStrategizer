#include "TraceEx.h"
#include "GameStateEx.h"

TraceEx::TraceEx(unsigned long p_gameCycle, int p_abstractActionId, const PlanStepParameters& p_abstractParametersId, 
				 const GoalSatisfactionEx& p_goalSatisfaction, GameStateEx* p_gameState, PlayerType p_player, PlayerType p_enemy)
				 : _gameCycle(p_gameCycle), _abstractActionId(p_abstractActionId), _abstractParametersId(p_abstractParametersId),
				   _goalSatisfaction(p_goalSatisfaction), _gameState(p_gameState), _player(p_player), _enemy(p_enemy) {}
//----------------------------------------------------------------------------------------------
TraceEx::TraceEx(unsigned long p_gameCycle, int p_abstractActionId, const PlanStepParameters& p_abstractParametersId, 
                  GameStateEx* p_gameState, PlayerType p_player, PlayerType p_enemy)
                 : _gameCycle(p_gameCycle), _abstractActionId(p_abstractActionId), _abstractParametersId(p_abstractParametersId),
                   _gameState(p_gameState), _player(p_player), _enemy(p_enemy) {}
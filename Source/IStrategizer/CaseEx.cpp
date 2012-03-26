#include "CaseEx.h"

#ifndef ENGINEDATA_H
	#include "EngineData.h"
#endif

CaseEx::CaseEx(PlanGraph* p_planGraph, GoalEx* p_goal, GameStateEx* p_gameState, int p_trialCount, int p_successCount)
               : _planGraph(p_planGraph), _goal(p_goal), _gameState(p_gameState),
               _trialCount(p_trialCount), _successCount(p_successCount) { }
//----------------------------------------------------------------------------------------------------------------------------------------------------
void CaseEx::InitializeAddressesAux()
{
    AddMemberAddress(5,
        &_planGraph,
        &_goal,
        &_gameState,
        &_trialCount,
        &_successCount);
}
//----------------------------------------------------------------------------------------------
CaseEx* CaseEx::From(CookedPlan* p_cookedPlan)
{
    return new CaseEx(p_cookedPlan->pPlan, p_cookedPlan->Goal, p_cookedPlan->gameState, 1, 1);
}
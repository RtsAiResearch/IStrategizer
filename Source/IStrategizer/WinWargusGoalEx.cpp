#include "WinWargusGoalEx.h"
#include "EntityClassExist.h"
#include "And.h"
#include "False.h"
#include <cassert>
#include "TypesMetaData.h"

WinWargusGoalEx::WinWargusGoalEx() : GoalEx(GOALEX_WinGame)
{
	_params[PARAM_StrategyTypeId]	= STRTYPE_START;
}
//----------------------------------------------------------------------------------------------
WinWargusGoalEx::WinWargusGoalEx(const PlanStepParameters& p_parameters): GoalEx(GOALEX_WinGame, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void WinWargusGoalEx::InitializeSuccessConditions()
{
	vector<PlayerType> m_playerIds;
	PlayerType m_enginePlayer;

	m_enginePlayer = PLAYER_Self;
	g_Assist.GetCurrentPlayers(m_playerIds);

	vector<Expression*> m_terms;
	int                 m_index = 0;

	m_terms.resize(m_playerIds.size() - 1);
	for (int m_playerIndex = 0; m_playerIndex < m_playerIds.size(); m_playerIndex++)
	{
		if(m_playerIds[m_playerIndex] == m_enginePlayer)
			continue;
		m_terms[m_index++] = new False;
		//m_terms[m_index++] = new CheckEntityClassAttribute((PlayerType)m_playerIds[m_playerIndex], 0, ECATTR_Count, RELOP_Equal, 0);
	}
	_successCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void WinWargusGoalEx::InitializePostConditions()
{

}

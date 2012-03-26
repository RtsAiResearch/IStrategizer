#include "BuildAction.h"
#include "EntityClassExist.h"
#include "ResourceExist.h"
#include "CheckEntityObjectAttribute.h"
#include "EntityObjectExist.h"
#include "Misc.h"
#include "And.h"
#include "Vector2.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "AbstractAdapter.h"
#include "CellFeature.h"
#include "CaseBasedReasonerEx.h"

#ifndef DATAMESSAGE_H
#include "DataMessage.h"
#endif
#ifndef FALSE_H
#include "False.h"
#endif

using namespace OLCBP;

BuildAction::BuildAction() : Action(ACTIONEX_Build), _targetCell(NULL)
{
	_params[PARAM_BuildingClassId]	= ECLASS_START;
	_params[PARAM_WorkerClassId]	= ECLASS_START;
}
//----------------------------------------------------------------------------------------------
BuildAction::BuildAction(const PlanStepParameters& p_parameters, CellFeature* p_targetCell)
: Action(ACTIONEX_Build, p_parameters), _targetCell(p_targetCell)
{

}
//----------------------------------------------------------------------------------------------
void BuildAction::InitializePreConditions()
{
	Cost							m_cost;
	int								aIndex, i;
	vector<EntityClassType>			m_requiredBuildings;
	map<EntityObjectAttribute, int>	m_workerAttributes;

	g_Assist.GetRequiredResources(PLAYER_Self, _params[PARAM_BuildingClassId], m_cost);
	g_Assist.GetRequiredBuildings(PLAYER_Self, _params[PARAM_BuildingClassId], m_requiredBuildings);

	vector<Expression*> m_terms;

	m_terms.resize(4 + m_requiredBuildings.size());

	m_terms[aIndex = 0] = new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_WorkerClassId], 1, m_workerAttributes, true);
	m_terms[++aIndex] = new ResourceExist(PLAYER_Self, RESOURCE_Primary, m_cost.Gold);
	m_terms[++aIndex] = new ResourceExist(PLAYER_Self, RESOURCE_Secondary, m_cost.Wood);
	m_terms[++aIndex] = new ResourceExist(PLAYER_Self, RESOURCE_Secondary, m_cost.Wood);

	for(i = 0; i < m_requiredBuildings.size(); ++i)
	{
		m_terms[++aIndex] = new EntityClassExist(PLAYER_Self, (EntityClassType)m_requiredBuildings[i], 1);
	}
	_preCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void BuildAction::InitializeAliveConditions()
{
	// FIXME: object id should get from memory address
	_aliveCondition = new And();
	EntityClassExist* m_cond = (EntityClassExist*)_preCondition->operator [](0);
	_aliveCondition->AddExpression(new EntityObjectExist(PLAYER_Self, m_cond->GetEntityIdByIndex(0)));
}
//----------------------------------------------------------------------------------------------
void BuildAction::InitializeSuccessConditions()
{
	// FIXME: object id should get from memory address
	_successCondition = new And();
	_successCondition->AddExpression(new False);
}
//----------------------------------------------------------------------------------------------
void BuildAction::InitializePostConditions()
{
	vector<Expression*> m_terms;

	//if (_params[PARAM_BuildingClassId] == ECLASS_PigFarm)
	//{
	//	m_terms.push_back( new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_BuildingClassId], 1) );

	//	for (int k = 0; k < 4; ++k)
	//	{
	//		m_terms.push_back( new ResourceExist(PLAYER_Self, RESOURCE_Supply, 1) );
	//	}
	//}
	//// Should add same for GreatHall as it adds 1 food supply
	//else
	//{
	//	m_terms.push_back(new EntityClassExist(PLAYER_Self, 
	//		(EntityClassType)_params[PARAM_BuildingClassId], 1));
	//}

	_postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void BuildAction::HandleMessage(Message* p_msg, bool& p_consumed)
{
	if(State() == ESTATE_Executing && p_msg->MessageTypeID() == MSG_BuildingBuilt) 
	{
		DataMessage<PlanStepParameters>* m_msg = static_cast<DataMessage<PlanStepParameters>*>(p_msg);
		EntityClassExist* m_cond = (EntityClassExist*)_preCondition->operator [](0);
		if(const_cast<PlanStepParameters*>(m_msg->Data())->operator [](PARAM_WorkerObjectId) == m_cond->GetEntityIdByIndex(0))
		{
			_buildingObjectId = const_cast<PlanStepParameters*>(m_msg->Data())->operator [](PARAM_BuildingObjectId);
			_aliveCondition->AddExpression(new EntityObjectExist(PLAYER_Self, &_buildingObjectId));

			_successCondition->Clear();
			_successCondition->AddExpression(new CheckEntityObjectAttribute(PLAYER_Self, &_buildingObjectId, EOATTR_State, RELOP_Equal, OBJSTATE_Idle));
			p_consumed = true;
		}
	}
}
//----------------------------------------------------------------------------------------------
int BuildAction::ExecuteAux(unsigned long p_cycles)
{
	Vector2 targetPosition = Vector2::Null(); // g_OnlineCaseBasedPlanner->Reasoner()->Adapter()->AdaptPositionForBuilding(_targetCell, 0, 0);
	EntityClassExist* m_cond = (EntityClassExist*)_preCondition->operator [](0);

	return g_Assist.ExecuteBuild((EntityClassType)_params[PARAM_BuildingClassId], 
		m_cond->GetEntityIdByIndex(0), 
		targetPosition, _buildingObjectId);
}
//----------------------------------------------------------------------------------------------
void BuildAction::Copy(IClonable* p_dest)
{
	Action::Copy(p_dest);

	BuildAction* m_dest = static_cast<BuildAction*>(p_dest);

	//_posDescription.Copy(&m_dest->_posDescription);
}

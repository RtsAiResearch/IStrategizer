#include "CaseGenerator.h"
#include "CaseBaseEx.h"
#include "GoalFactory.h"
#include "ActionFactory.h"
#include "GoalEx.h"
#include "Action.h"

using namespace IStrategizer;
using namespace std;

void CaseGenerator::GenBuildOrderCases()
{
	GenCollectPrimaryResourceCases();
	GenCollectSecondaryResourceCases();
	GenBuildInfraCases("Terran_Refinery", 1);
	GenBuildInfraCases("Terran_Barracks", 2);
	GenBuildInfraCases("Terran_Factory", 2);
	GenBuildInfraCases("Terran_Supply_Depot", 1);
	GenBuildInfraCases("Terran_Machine_Shop", 1);
	GenBuildInfraCases("Terran_Bunker", 2);
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenTrainOrderCases()
{
	GenTrainForceCases("Terran_SCV", MaxSCVs);
	GenTrainForceCases("Terran_Marine", 9);
	GenTrainForceCases("Terran_Medic", 4);
	GenTrainForceCases("Terran_Vulture", 6);
	GenTrainForceCases("Terran_Siege_Tank_Tank_Mode", 6);
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenCollectPrimaryResourceCases()
{
	/*
	Collect(X) = { X == 1 -> S(TrainForce(SCV),Gather(Primary))
	{ X > 1  -> S(Collect(X-1),TrainForce(SCV),Gather(Primary))
	*/
	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");

	for (unsigned numSCVs = 1; numSCVs <= MaxSCVs; ++numSCVs)
	{
		auto pCollectResourceCase = m_pCb->NewCase(GOALEX_CollectResource);
		pCollectResourceCase->Generated(true);

		pCollectResourceCase->Goal()->Parameter(PARAM_ResourceId, RESOURCE_Primary);
		pCollectResourceCase->Goal()->Parameter(PARAM_Amount, numSCVs);

		// GatherResource(Primary,DistanceToBase=25)
		auto pSCVGatherA = g_ActionFactory.GetAction(ACTIONEX_GatherResource, false);
		pSCVGatherA->Parameter(PARAM_ResourceId, RESOURCE_Primary);
		pSCVGatherA->Parameter(PARAM_DistanceToBase, 25);
		pCollectResourceCase->Plan()->AddNode(pSCVGatherA, pSCVGatherA->Id());

		if (numSCVs > 1)
		{
			// CollectResource(Primary,numSCVs-1)
			auto pCollectResource = g_GoalFactory.GetGoal(GOALEX_CollectResource, false);
			pCollectResource->Parameter(PARAM_ResourceId, RESOURCE_Primary);
			pCollectResource->Parameter(PARAM_Amount, numSCVs - 1);
			pCollectResourceCase->Plan()->AddNode(pCollectResource, pCollectResource->Id());

			pCollectResourceCase->Plan()->AddEdge(pCollectResource->Id(), pSCVGatherA->Id());
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenCollectSecondaryResourceCases()
{
	/*
	Collect(X) = { X == 1 -> S(BuildInfra(Refinery),TrainForce(SCV),Gather(Secondary))
	{ X > 1  -> S(Collect(X-1),BuildInfra(Refinery),TrainForce(SCV),Gather(Secondary))
	*/
	const unsigned MaxSecondarySCVs = 3;
	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");
	EntityClassType refineryType = (EntityClassType)m_idLookup.GetBySecond("Terran_Refinery");

	for (unsigned numSCVs = 1; numSCVs <= MaxSecondarySCVs; ++numSCVs)
	{
		auto pCollectResourceCase = m_pCb->NewCase(GOALEX_CollectResource);
		pCollectResourceCase->Generated(true);

		pCollectResourceCase->Goal()->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
		pCollectResourceCase->Goal()->Parameter(PARAM_Amount, numSCVs);

		// GatherResource(Secondary,DistanceToBase=25)
		auto pSCVGatherA = g_ActionFactory.GetAction(ACTIONEX_GatherResource, false);
		pSCVGatherA->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
		pSCVGatherA->Parameter(PARAM_DistanceToBase, 25);
		pCollectResourceCase->Plan()->AddNode(pSCVGatherA, pSCVGatherA->Id());

		if (numSCVs > 1)
		{
			// CollectResource(Secondary,numSCVs-1)
			auto pCollectResourceG = g_GoalFactory.GetGoal(GOALEX_CollectResource, false);
			pCollectResourceG->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
			pCollectResourceG->Parameter(PARAM_Amount, numSCVs - 1);
			pCollectResourceCase->Plan()->AddNode(pCollectResourceG, pCollectResourceG->Id());

			pCollectResourceCase->Plan()->AddEdge(pCollectResourceG->Id(), pSCVGatherA->Id());
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenRecursiveGoalActionCases(GoalType goalType, const PlanStepParameters& goalParams, ActionType actionType, const PlanStepParameters& actionParams)
{
	unsigned maxCases = goalParams.at(PARAM_Amount);

	for (unsigned caseOrder = 1; caseOrder <= maxCases; ++caseOrder)
	{
		auto pCase = m_pCb->NewCase(goalType);
		pCase->Generated(true);

		pCase->Goal()->Parameters(goalParams);
		pCase->Goal()->Parameter(PARAM_Amount, caseOrder);

		auto pAction = g_ActionFactory.GetAction(actionType, false);
		pAction->Parameters(actionParams);
		pCase->Plan()->AddNode(pAction, pAction->Id());

		if (caseOrder > 1)
		{
			auto pGoal = g_GoalFactory.GetGoal(goalType, false);
			pGoal->Parameters(goalParams);
			pGoal->Parameter(PARAM_Amount, caseOrder - 1);
			pCase->Plan()->AddNode(pGoal, pGoal->Id());

			pCase->Plan()->AddEdge(pGoal->Id(), pAction->Id());
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenTrainForceCases(const char* pBwapiUnitName, unsigned amount)
{
	EntityClassType unitType = (EntityClassType)m_idLookup.GetBySecond(pBwapiUnitName);
	PlanStepParameters goalParams = map<ParameterType, int>{ { PARAM_EntityClassId, unitType }, { PARAM_Amount, amount } };
	PlanStepParameters actionParams = map<ParameterType, int>{{ PARAM_EntityClassId, unitType }};

	GenRecursiveGoalActionCases(GOALEX_TrainForce, goalParams, ACTIONEX_Train, actionParams);
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenBuildInfraCases(const char* pBwapiUnitName, unsigned amount)
{
	EntityClassType unitType = (EntityClassType)m_idLookup.GetBySecond(pBwapiUnitName);
	PlanStepParameters goalParams = map<ParameterType, int>{{ PARAM_EntityClassId, unitType }, { PARAM_Amount, amount } };
	PlanStepParameters actionParams = map<ParameterType, int>{{ PARAM_EntityClassId, unitType }, { PARAM_DistanceToBase, 25 }};

	GenRecursiveGoalActionCases(GOALEX_BuildInfrastructure, goalParams, ACTIONEX_Build, actionParams);
}

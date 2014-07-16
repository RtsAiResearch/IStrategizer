#include "CaseGenerator.h"
#include "CaseBaseEx.h"
#include "GoalFactory.h"
#include "ActionFactory.h"
#include "GoalEx.h"
#include "Action.h"

using namespace IStrategizer;
using namespace std;

CaseEx* CaseGenerator::NewCase(_In_ GoalType caseGoal)
{
	_ASSERTE(m_pCb);
	GoalEx* pNewGoal = g_GoalFactory.GetGoal(caseGoal, false);
	CaseEx* pNewCase = new CaseEx(new OlcbpPlan, pNewGoal, nullptr, 1, 1);
	m_pCb->CaseContainer.push_back(pNewCase);

	return pNewCase;
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenBuildOrderCases()
{
	GenCollectPrimaryResourceCases();
	GenCollectSecondaryResourceCases();
	GenSCVTrainForceCases();
	GenBuildRefineryCases();
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenTrainOrderCases()
{

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
		auto pCollectResourceCase = NewCase(GOALEX_CollectResource);

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
void CaseGenerator::GenSCVTrainForceCases()
{
	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");

	for (unsigned numSCVs = 1; numSCVs <= MaxSCVs; ++numSCVs)
	{
		auto pTrainScvCase = NewCase(GOALEX_TrainForce);
		pTrainScvCase->Goal()->Parameter(PARAM_EntityClassId, scvType);
		pTrainScvCase->Goal()->Parameter(PARAM_ObjectStateType, OBJSTATE_DontCare);
		pTrainScvCase->Goal()->Parameter(PARAM_Amount, numSCVs);

		auto pTrainScvA = g_ActionFactory.GetAction(ACTIONEX_Train, false);
		pTrainScvA->Parameter(PARAM_EntityClassId, scvType);
		pTrainScvCase->Plan()->AddNode(pTrainScvA, pTrainScvA->Id());

		if (numSCVs > 1)
		{
			// TrainForce(Primary,numSCVs-1)
			auto pTrainScvG = g_GoalFactory.GetGoal(GOALEX_TrainForce, false);
			pTrainScvG->Parameter(PARAM_EntityClassId, scvType);
			pTrainScvG->Parameter(PARAM_ObjectStateType, OBJSTATE_DontCare);
			pTrainScvG->Parameter(PARAM_Amount, numSCVs - 1);
			pTrainScvCase->Plan()->AddNode(pTrainScvG, pTrainScvG->Id());

			pTrainScvCase->Plan()->AddEdge(pTrainScvG->Id(), pTrainScvA->Id());
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
		auto pCollectResourceCase = NewCase(GOALEX_CollectResource);

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
void CaseGenerator::GenBuildRefineryCases()
{
	auto pBuildRefineryCase = NewCase(GOALEX_BuildInfrastructure);

	EntityClassType refineryType = (EntityClassType)m_idLookup.GetBySecond("Terran_Refinery");

	pBuildRefineryCase->Goal()->Parameter(PARAM_EntityClassId, refineryType);
	pBuildRefineryCase->Goal()->Parameter(PARAM_Amount, 1);

	auto pBuildRefineryA = g_ActionFactory.GetAction(ACTIONEX_Build, false);
	pBuildRefineryA->Parameter(PARAM_EntityClassId, (int)refineryType);
	pBuildRefineryA->Parameter(PARAM_DistanceToBase, 25);
	pBuildRefineryCase->Plan()->AddNode(pBuildRefineryA, pBuildRefineryA->Id());
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenRecursiveGoalActionCases(GoalType goalType, const PlanStepParameters& goalParams, ActionType actionType, const PlanStepParameters& actionParams)
{
	unsigned maxCases = goalParams.at(PARAM_Amount);

	for (unsigned caseOrder = 1; caseOrder <= maxCases; ++caseOrder)
	{
		auto pCase = NewCase(goalType);
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
void CaseGenerator::GetTrainForceCases(const char* pBwapiUnitName, unsigned amount)
{
	EntityClassType unitType = (EntityClassType)m_idLookup.GetBySecond(pBwapiUnitName);
	PlanStepParameters goalParams = map<ParameterType, int>{ { PARAM_EntityClassId, unitType }, { PARAM_Amount, amount } };
	PlanStepParameters actionParams = map<ParameterType, int>{{ PARAM_EntityClassId, unitType }};

	GenRecursiveGoalActionCases(GOALEX_TrainForce, goalParams, ACTIONEX_Train, actionParams);
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::DeleteAllGeneratedCases()
{

}
#include "CaseGenerator.h"
#include "CaseBaseEx.h"
#include "GoalFactory.h"
#include "ActionFactory.h"
#include "GoalEx.h"
#include "Action.h"

using namespace IStrategizer;

CaseEx* CaseGenerator::NewCase(_In_ GoalType caseGoal)
{
	_ASSERTE(m_pCb);
	GoalEx* pNewGoal = g_GoalFactory.GetGoal(caseGoal, false);
	CaseEx* pNewCase = new CaseEx(new OlcbpPlan, pNewGoal, nullptr, 1, 1);
	m_pCb->CaseContainer.push_back(pNewCase);

	return pNewCase;
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenResourceMgmtCases()
{
	GenCollectPrimaryResourceCases();
	GenCollectSecondaryResourceCases();
	GenSCVTrainForceCases();
	GenBuildRefineryCases();
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenCollectPrimaryResourceCases()
{
	/*
	Collect(X) = { X == 1 -> S(TrainForce(SCV),Gather(Primary))
	{ X > 1  -> S(Collect(X-1),TrainForce(SCV),Gather(Primary))
	*/
	const unsigned MaxSCVs = 15;
	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");

	for (unsigned numSCVs = 1; numSCVs <= MaxSCVs; ++numSCVs)
	{
		auto pCollectResourceCase = NewCase(GOALEX_CollectResource);

		pCollectResourceCase->Goal()->Parameter(PARAM_ResourceId, RESOURCE_Primary);
		pCollectResourceCase->Goal()->Parameter(PARAM_Amount, numSCVs);

		// TrainForce(SCV,Idle,1)
		auto pSCVTrainForceG = g_GoalFactory.GetGoal(GOALEX_TrainForce, false);
		pSCVTrainForceG->Parameter(PARAM_EntityClassId, scvType);
		pSCVTrainForceG->Parameter(PARAM_ObjectStateType, OBJSTATE_Idle);
		pSCVTrainForceG->Parameter(PARAM_Amount, 1);
		pCollectResourceCase->Plan()->AddNode(pSCVTrainForceG, pSCVTrainForceG->Id());

		// GatherResource(Primary,DistanceToBase=25)
		auto pSCVGatherA = g_ActionFactory.GetAction(ACTIONEX_GatherResource, false);
		pSCVGatherA->Parameter(PARAM_ResourceId, RESOURCE_Primary);
		pSCVGatherA->Parameter(PARAM_DistanceToBase, 25);
		pCollectResourceCase->Plan()->AddNode(pSCVGatherA, pSCVGatherA->Id());

		pCollectResourceCase->Plan()->AddEdge(pSCVTrainForceG->Id(), pSCVGatherA->Id());

		if (numSCVs > 1)
		{
			// CollectResource(Primary,numSCVs-1)
			auto pCollectResource = g_GoalFactory.GetGoal(GOALEX_CollectResource, false);
			pCollectResource->Parameter(PARAM_ResourceId, RESOURCE_Primary);
			pCollectResource->Parameter(PARAM_Amount, numSCVs - 1);
			pCollectResourceCase->Plan()->AddNode(pCollectResource, pCollectResource->Id());

			pCollectResourceCase->Plan()->AddEdge(pCollectResource->Id(), pSCVTrainForceG->Id());
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenSCVTrainForceCases()
{
	auto pTrainForceCase = NewCase(GOALEX_TrainForce);

	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");

	pTrainForceCase->Goal()->Parameter(PARAM_EntityClassId, scvType);
	pTrainForceCase->Goal()->Parameter(PARAM_ObjectStateType, OBJSTATE_Idle);
	pTrainForceCase->Goal()->Parameter(PARAM_Amount, 1);

	auto pTrainSCV = g_ActionFactory.GetAction(ACTIONEX_Train, false);
	pTrainSCV->Parameter(PARAM_EntityClassId, (int)scvType);
	pTrainForceCase->Plan()->AddNode(pTrainSCV, pTrainSCV->Id());
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenCollectSecondaryResourceCases()
{
	/*
	Collect(X) = { X == 1 -> S(BuildInfra(Refinery),TrainForce(SCV),Gather(Secondary))
	{ X > 1  -> S(Collect(X-1),BuildInfra(Refinery),TrainForce(SCV),Gather(Secondary))
	*/
	const unsigned MaxSCVs = 3;
	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");
	EntityClassType refineryType = (EntityClassType)m_idLookup.GetBySecond("Terran_Refinery");

	for (unsigned numSCVs = 1; numSCVs <= MaxSCVs; ++numSCVs)
	{
		auto pCollectResourceCase = NewCase(GOALEX_CollectResource);

		pCollectResourceCase->Goal()->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
		pCollectResourceCase->Goal()->Parameter(PARAM_Amount, numSCVs);

		// BuildInfrastructure(Refinery,1)
		auto pBuildRefineryG = g_GoalFactory.GetGoal(GOALEX_BuildInfrastructure, false);
		pBuildRefineryG->Parameter(PARAM_EntityClassId, refineryType);
		pBuildRefineryG->Parameter(PARAM_Amount, 1);
		pCollectResourceCase->Plan()->AddNode(pBuildRefineryG, pBuildRefineryG->Id());

		// TrainForce(SCV,Idle,1)
		auto pSCVTrainForceG = g_GoalFactory.GetGoal(GOALEX_TrainForce, false);
		pSCVTrainForceG->Parameter(PARAM_EntityClassId, scvType);
		pSCVTrainForceG->Parameter(PARAM_ObjectStateType, OBJSTATE_Idle);
		pSCVTrainForceG->Parameter(PARAM_Amount, 1);
		pCollectResourceCase->Plan()->AddNode(pSCVTrainForceG, pSCVTrainForceG->Id());

		// GatherResource(Secondary,DistanceToBase=25)
		auto pSCVGatherA = g_ActionFactory.GetAction(ACTIONEX_GatherResource, false);
		pSCVGatherA->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
		pSCVGatherA->Parameter(PARAM_DistanceToBase, 25);
		pCollectResourceCase->Plan()->AddNode(pSCVGatherA, pSCVGatherA->Id());

		pCollectResourceCase->Plan()->AddEdge(pBuildRefineryG->Id(), pSCVTrainForceG->Id());
		pCollectResourceCase->Plan()->AddEdge(pSCVTrainForceG->Id(), pSCVGatherA->Id());

		if (numSCVs > 1)
		{
			// CollectResource(Secondary,numSCVs-1)
			auto pCollectResource = g_GoalFactory.GetGoal(GOALEX_CollectResource, false);
			pCollectResource->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
			pCollectResource->Parameter(PARAM_Amount, numSCVs - 1);
			pCollectResourceCase->Plan()->AddNode(pCollectResource, pCollectResource->Id());

			pCollectResourceCase->Plan()->AddEdge(pCollectResource->Id(), pBuildRefineryG->Id());
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

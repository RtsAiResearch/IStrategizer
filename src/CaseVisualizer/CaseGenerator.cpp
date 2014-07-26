#include "CaseGenerator.h"
#include "CaseBaseEx.h"
#include "GoalFactory.h"
#include "ActionFactory.h"
#include "GoalEx.h"
#include "Action.h"
#include "GameType.h"
#include <queue>

using namespace IStrategizer;
using namespace std;

void CaseGenerator::GenBuildOrderCases()
{
	GenCollectPrimaryResourceCases();
	GenCollectSecondaryResourceCases();
	GenBuildInfraCases("Terran_Barracks", 2);
	GenBuildInfraCases("Terran_Factory", 2);
	GenBuildInfraCases("Terran_Machine_Shop", 2);
    GenBuildInfraCases("Terran_Armory", 2);
    GenBuildInfraCases("Terran_Command_Center", 2);
    GenBuildInfraCases("Terran_Physics_Lab", 2);
    GenBuildInfraCases("Terran_Science_Facility", 1);
    GenBuildInfraCases("Terran_Covert_Ops", 2);
    GenBuildInfraCases("Terran_Starport", 2);
	GenBuildInfraCases("Terran_Supply_Depot", 2);
	GenBuildInfraCases("Terran_Bunker", 3);
	GenBuildInfraCases("Terran_Academy", 1);
	GenBuildInfraCases("Terran_Refinery", 1);
    GenBuildInfraCases("Terran_Engineering_Bay", 1);
    GenBuildInfraCases("Terran_Missile_Turret", 4);
}
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::GenTrainOrderCases()
{
	GenTrainForceCases("Terran_SCV", MaxSCVs);
	GenTrainForceCases("Terran_Marine", 12);
	GenTrainForceCases("Terran_Medic", 6);
	GenTrainForceCases("Terran_Vulture", 12);
	GenTrainForceCases("Terran_Siege_Tank_Tank_Mode", 6);
	GenTrainForceCases("Terran_Firebat", 9);
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
//////////////////////////////////////////////////////////////////////////
void CaseGenerator::CalcTrainArmyCaseParams(_In_ CaseEx* pCase)
{
    if (pCase->Goal()->StepTypeId() != GOALEX_TrainArmy)
        return;

    auto nodes = pCase->Plan()->GetNodes();
    
    map<EntityClassType, int> resultantAmounts;
    EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");
    for (auto nodeId : nodes)
    {
        auto pPlanstep = pCase->Plan()->GetNode(nodeId);

        if (pPlanstep->StepTypeId() != GOALEX_TrainForce)
            continue;

        if ((EntityClassType)pPlanstep->Parameter(PARAM_EntityClassId) == scvType)
            continue;

        auto type = (EntityClassType)pPlanstep->Parameter(PARAM_EntityClassId);
        auto amount = pPlanstep->Parameter(PARAM_Amount);

        resultantAmounts[type] = max(amount, resultantAmounts[type]);
    }

    int hpAcc = 0;
    int dmgAcc = 0;

    for (auto& r : resultantAmounts)
    {
        auto pType = g_Game->GetEntityType(r.first);

        hpAcc += (pType->P(TP_MaxHp) * r.second);
        dmgAcc += (pType->P(TP_GroundAttack) * r.second);
    }

    PlanStepParameters newParams;
    newParams[PARAM_AlliedAttackersTotalHP] = hpAcc;
    newParams[PARAM_AlliedAttackersTotalDamage] = dmgAcc;

    pCase->Goal()->Parameters(newParams);
}
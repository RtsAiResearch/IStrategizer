#ifndef ONLINECASEBASEDPLANNEREX_H
#include "OnlineCaseBasedPlannerEx.h"
#endif
#ifndef ABSTRACTRETAINER_H
#include "AbstractRetainer.h"
#endif
#ifndef ABSTRACTRETRIEVER_H
#include "AbstractRetriever.h"
#endif
#ifndef ABSTRACTREVISER_H
#include "AbstractReviser.h"
#endif
#ifndef ABSTRACTADAPTER_H
#include "AbstractAdapter.h"
#endif
#ifndef CASEBASEEX_H
#include "CaseBaseEx.h"
#endif
#ifndef RETAINEREX_H
#include "RetainerEx.h"
#endif
#ifndef RETRIEVEREX_H
#include "RetrieverEx.h"
#endif
#ifndef ADAPTEREX_H
#include "AdapterEx.h"
#endif
#ifndef REVISER_H
#include "Reviser.h"
#endif
#ifndef CASEBASEDREASONEREX_H
#include "CaseBasedReasonerEx.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef ONLINEPLANEXPANSIONEXECUTIONEX_H
#include "OnlinePlanExpansionExecutionEx.h"
#endif

#include "RtsGame.h"
#include "WorldMap.h"
#include "CellFeature.h"
#include "Vector2.h"
#include "AttackGroundAction.h"
#include "CellFeature.h"
#include "BuildAction.h"

using namespace OLCBP;

OLCBP::OnlineCaseBasedPlannerEx *g_OnlineCaseBasedPlanner = NULL;

OnlineCaseBasedPlannerEx::OnlineCaseBasedPlannerEx() : _caseBasedReasoner(NULL), _onlineExpansionExecution(NULL)
{
}
//----------------------------------------------------------------------------------------------
void OnlineCaseBasedPlannerEx::Init(GoalEx *p_initialGoal)
{
    AbstractRetainer*   m_retainer  = new RetainerEx(g_CaseBasePath);
    AbstractRetriever*  m_retriever = new RetrieverEx(m_retainer);
    AbstractReviser*    m_revisor   = new Reviser();
    AbstractAdapter*    m_adapter   = new AdapterEx();
    _caseBasedReasoner = new CaseBasedReasonerEx(
        m_retainer,
        m_revisor,
        m_retriever,
        m_adapter);

    _caseBasedReasoner->Initialize();

    _onlineExpansionExecution = new OnlinePlanExpansionExecutionEx(p_initialGoal, _caseBasedReasoner);
}
//----------------------------------------------------------------------------------------------
void OnlineCaseBasedPlannerEx::Update(unsigned long p_gameCycle)
{
	//static unsigned long lastGameCycle = INT_MAX;
	//if(p_gameCycle < lastGameCycle)
	//{
	//	PlanStepParameters aParams;

	//	aParams[PARAM_PlayerId] = PLAYER_Self;
	//	aParams[PARAM_WorkerClassId] = ECLASS_Peon;
	//	aParams[PARAM_BuildingClassId] = ECLASS_PigFarm;
	//	
	//	WorldModel::CellFeature vv;
	//	vv.Clear();
	//	vv.m_alliedForceDescription.m_numberOfUnits = 1;
	//	vv.m_alliedForceDescription.m_totalHP = 30;
	//	BuildAction b(aParams,&vv);
	//	b.InitializeConditions();
	//	g_RtsGame->Map()->Update();
	//	b.PrepareForExecution();

	//	b.Execute();

	//	aParams[PARAM_PlayerId] = PLAYER_Self;
	//	aParams[PARAM_EntityClassId] = ECLASS_Peon;
	//	WorldModel::CellFeature v;
	//	v.m_enemyForceDescription.m_numberOfUnits = 1;
	//	v.m_enemyForceDescription.m_totalHP = 30;
	//	AttackGroundAction a(aParams, &v);
	//	a.InitializeConditions();
	//	g_RtsGame->Map()->Update();
	//	a.PrepareForExecution();
	//	a.Execute();

	//	lastGameCycle = p_gameCycle;
	//}

	_onlineExpansionExecution->Update(p_gameCycle);
}
//----------------------------------------------------------------------------------------------
OnlineCaseBasedPlannerEx::~OnlineCaseBasedPlannerEx()
{
    Toolbox::MemoryClean(_caseBasedReasoner);
    Toolbox::MemoryClean(_onlineExpansionExecution);
}
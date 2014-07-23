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
#include "OnlinePlanExpansionExecution.h"
#endif

#include "RtsGame.h"
#include "WorldMap.h"
#include "CellFeature.h"
#include "Vector2.h"
#include "AttackGroundAction.h"
#include "CellFeature.h"

using namespace IStrategizer;

OnlineCaseBasedPlannerEx *g_OnlineCaseBasedPlanner = nullptr;

OnlineCaseBasedPlannerEx::OnlineCaseBasedPlannerEx() : 
    _caseBasedReasoner(nullptr),
    _onlineExpansionExecution(nullptr) {}
//----------------------------------------------------------------------------------------------
bool OnlineCaseBasedPlannerEx::Init()
{
    AbstractRetainer* m_retainer = new RetainerEx();
    AbstractRetriever* m_retriever = new RetrieverEx(m_retainer);
    AbstractReviser* m_revisor = new Reviser();
    AbstractAdapter* m_adapter = new AdapterEx();
    _caseBasedReasoner = new CaseBasedReasonerEx(m_retainer, m_revisor, m_retriever, m_adapter);

    if (!_caseBasedReasoner->Init())
    {
        LogError("Casebased reasoner initialization failed");
        return false;
    }

    _onlineExpansionExecution = new OnlinePlanExpansionExecution(_caseBasedReasoner);

    return true;
}
//----------------------------------------------------------------------------------------------
void OnlineCaseBasedPlannerEx::Update(_In_ RtsGame& game)
{
    _onlineExpansionExecution->Update(game);
}
//----------------------------------------------------------------------------------------------
OnlineCaseBasedPlannerEx::~OnlineCaseBasedPlannerEx()
{
    Toolbox::MemoryClean(_caseBasedReasoner);
    Toolbox::MemoryClean(_onlineExpansionExecution);
}
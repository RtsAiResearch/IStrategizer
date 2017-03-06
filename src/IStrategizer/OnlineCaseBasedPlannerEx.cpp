#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBaseEx.h"
#include "RetainerEx.h"
#include "Reviser.h"
#include "RetrieverEx.h"
#include "AdapterEx.h"
#include "CaseBasedReasonerEx.h"
#include "Toolbox.h"
#include "OnlinePlanExpansionExecution.h"
#include "RtsGame.h"
#include "WorldMap.h"
#include "CellFeature.h"
#include "Vector2.h"

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
void OnlineCaseBasedPlannerEx::Update()
{
    _onlineExpansionExecution->Update();
}
//----------------------------------------------------------------------------------------------
OnlineCaseBasedPlannerEx::~OnlineCaseBasedPlannerEx()
{
    Toolbox::MemoryClean(_caseBasedReasoner);
    Toolbox::MemoryClean(_onlineExpansionExecution);
}
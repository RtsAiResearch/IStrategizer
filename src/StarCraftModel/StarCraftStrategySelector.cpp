#include "StarCraftStrategySelector.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "CaseBasedReasonerEx.h"
#include "AbstractRetriever.h"
#include "CaseEx.h"

using namespace IStrategizer;

void StarCraftStrategySelector::SelectGameOpening(_In_ RtsGame& game, _Out_ PlanStepParameters& trainArmyParams) const
{
    LogActivity(SelectGameOpening);

    AbstractRetriever::RetrieveOptions opt;

    opt.CaseName = OPENING_TVP_Vultures_Rush;

    auto pCase = g_OnlineCaseBasedPlanner->Reasoner()->Retriever()->Retrieve(opt);

    if (pCase != nullptr)
    {
        trainArmyParams = pCase->Goal()->Parameters();
    }
}
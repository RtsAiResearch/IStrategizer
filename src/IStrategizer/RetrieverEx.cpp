#include "RetrieverEx.h"
#include "CaseEx.h"
#include "GoalEx.h"
#include "MathHelper.h"
#include "CaseBaseEx.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GoalFactory.h"
#include "AbstractRetainer.h"
#include "CaseBaseEx.h"
#include "Logger.h"
#include "RtsGame.h"
#include <map>
#include <functional>
#include <string>

using namespace std;
using namespace IStrategizer;
using namespace Serialization;

void CasebaseNumericalLimits::ComputeLimits(_In_ const CaseBaseEx& cb)
{
    LogInfo("Computing case goal parameters numerical limits");

    for (auto pCase : cb.CaseContainer)
    {
        auto& paramSet = pCase->Goal()->Parameters();

        for (auto param : paramSet)
        {
            if (m_goalParamsMinMax.count(param.first) == 0)
            {
                auto& val = m_goalParamsMinMax[param.first];
                val.Min = param.second;
                val.Max = param.second;
                val.Diff = 0;
            }
            else
            {
                auto& val = m_goalParamsMinMax[param.first];
                val.Min = min(val.Min, param.second);
                val.Max = max(val.Max, param.second);
                val.Diff = val.Max - val.Min;
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
float CasebaseNumericalLimits::Normalize(_In_ const MinMaxVal& limit, _In_ int val)
{
    // Make sure that val is within the limit for stability
    val = MathHelper::Clamp(val, limit.Min, limit.Max);
    // Normalize val in the continuous range [0, 1]
    return (float)(val - limit.Min) / (float)limit.Diff;
}
//----------------------------------------------------------------------------------------------
RetrieverEx::RetrieverEx(AbstractRetainer *p_pRetainer) : 
    AbstractRetriever(p_pRetainer, "Retriever") 
{}
//----------------------------------------------------------------------------------------------
bool RetrieverEx::Init()
{
    LogInfo("Initializing retriever...");

    m_limits.ComputeLimits(*m_pRetainer->CaseBase());

    return true;
}
//----------------------------------------------------------------------------------------------
shared_ptr<GoalEx> RetrieverEx::GetGoal(_In_ GoalType type, _In_ const PlanStepParameters& params)
{
    return shared_ptr<GoalEx>(g_GoalFactory.GetGoal(type, params, false));
}
//----------------------------------------------------------------------------------------------
float RetrieverEx::GoalDistance(const GoalEx* pCaseGoal, const AbstractRetriever::RetrieveOptions& options)
{
    // returns a value between 0 and 1.0, where 1 means identical and 0 means totally different
    // PARAMETER-DISTANCe PS(g1, g2)
    //  foreach param p1 in g1 and param p2 in g2
    //      distance += abs(p1 - p2)
    //

    _ASSERTE(pCaseGoal->StepTypeId() == options.GoalTypeId);

    float paramDistance = 0.0;

    auto& paramsG1 = pCaseGoal->Parameters();
    auto& paramsG2 = options.Parameters;

    // Use Manhattan distance between all params
    if (paramsG1.size() > 0 && paramsG2.size() > 0)
    {
        _ASSERTE(paramsG1.size() == paramsG2.size());

        for(PlanStepParameters::const_iterator itrG1 = paramsG1.begin(), itrG2 = paramsG2.begin();
            itrG1 != paramsG1.end();
            ++itrG1, ++itrG2)
        {
            // Params should be of the same type
            _ASSERTE(itrG1->first == itrG2->first);

            // Continuous param value
            if (ISREALVAL(ParameterType, itrG1->first))
            {
                auto& paramLimit = m_limits.GetLimit(itrG1->first);
                // absolute difference between normalized values in range [0, 1]
                float pv1= m_limits.Normalize(paramLimit, itrG1->second);
                float pv2 = m_limits.Normalize(paramLimit, itrG2->second);
                paramDistance += fabsf(pv1 - pv2);
            }
            // Discrete param values
            else
            {
                paramDistance += (itrG1->second == itrG2->second ? 0.0f : 1.0f);
            }
        }
    }

    return paramDistance;
}
//----------------------------------------------------------------------------------------------
float RetrieverEx::StateDistance(RtsGame* pCaseGameState, const AbstractRetriever::RetrieveOptions& options)
{
    // STATE-SIMILARITY(gs1, gs2)
    // distance = 0
    // for i: 0 to d
    // // distance += ((gs1[i] - gs2[i]) /  (1 + max(gs1[i], gs2[i]))) ^ 2
    // 
    // return 1 / (sqrt(distance) + 1.0)
    /*float distance = 0;
    float diff;
    float maxDiff;
    ShallowFeaturesEx& v1 = const_cast<GameStateEx*>(p_gs1)->ShallowFeatures();
    ShallowFeaturesEx& v2 = const_cast<GameStateEx*>(p_gs2)->ShallowFeatures();

    for(int i = 0, size = v1.size(); i < size; ++i)
    {
    diff = v1[i] - v2[i];
    maxDiff = 1.0f + max(v1[i], v2[i]);
    distance += pow(diff / maxDiff, 2);
    }

    return 1.0f / (sqrt(distance + 1.0f));*/

    pCaseGameState->Init();
    /*RtsGameModel currentState(*options.pGameState);
    RtsGameModel caseState(*pCaseGameState);

    return currentState.Distance(caseState, &RtsGame::DefaultWeightModel);*/
    return pCaseGameState->Distance(options.pGameState, &RtsGame::DefaultWeightModel);
}
//----------------------------------------------------------------------------------------------
float RetrieverEx::CaseDistance(const CaseEx* pCase, const AbstractRetriever::RetrieveOptions& options)
{
    float goalDistance = GoalDistance(pCase->Goal(), options);

    return goalDistance;
}
//----------------------------------------------------------------------------------------------
CaseEx* RetrieverEx::Retrieve(const AbstractRetriever::RetrieveOptions& options)
{
    if (!options.CaseName.empty())
    {
        LogInfo("Retreiving case by name '%s'", options.CaseName.c_str());
        return m_pRetainer->CaseBase()->FindByName(options.CaseName.c_str());
    }

    string goalDesc;

    if (!options.Parameters.empty())
    {
        auto pGoal = GetGoal(options.GoalTypeId, options.Parameters);
        goalDesc = pGoal->ToString();
        LogInfo("Retrieving case for parameterized goal={%s} and current game-state", goalDesc.c_str());
    }
    else
    {
        goalDesc = Enums[options.GoalTypeId];
        LogInfo("Retrieving case for non-parameterized goal={%s} and current game-state", goalDesc.c_str());
    }

    auto& cases = m_pRetainer->CaseBase()->CaseContainer;

    if (cases.empty())
    {
        LogError("Case-base is empty, retrieval failed");
        return nullptr;
    }

    // order cases by distance in ascending order
    multimap<float, CaseEx*, less<float>> caseRelevanceTable;

    // Calculate the relevance between each case in the case-base and
    // the current situation using the goal and game-state params
    for(auto pCase : cases)
    {
        // Ignore all cases with different goal type because it does not
        // make sense to retrieve a WinGame case for TrainArmy goal for example
        // PERF: consider cluster cases by goal type to only search in goal type bucket
        if (options.GoalTypeId != pCase->Goal()->StepTypeId())
            continue;

        //if (options.ExcludedCases.count(pCase) > 0)
        //{
        //    LogInfo("Case %s is excluded from retrieval, excluding by goal instance", pCase->Goal()->ToString().c_str());
        //    continue;
        //}
        //else if (options.ExcludedGoalHashes.count(pCase->Goal()->Hash()))
        //{
        //    LogInfo("Case %s is excluded from retrieval, excluding by goal type hash", pCase->Goal()->ToString().c_str());
        //    continue;
        //}

        float caseRelevance = CaseDistance(pCase, options);
        caseRelevanceTable.insert(make_pair(caseRelevance, pCase));
    }

    if (caseRelevanceTable.empty())
    {
        LogInfo("Failed to retrieve any matching case for %s", Enums[options.GoalTypeId]);
        return nullptr;
    }

    CaseEx* bestCase = caseRelevanceTable.begin()->second;

    LogInfo("Retrieved case '%s' for goal %s with performance=%f",
        bestCase->Goal()->ToString().c_str(),
        goalDesc.c_str(),
        (float)bestCase->SuccessCount() / (float)bestCase->TrialCount());

    return bestCase;
}

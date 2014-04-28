#ifndef RETRIEVEREX_H
#include "RetrieverEx.h"
#endif
#ifndef CASEEX_H
#include "CaseEx.h"
#endif
#ifndef GOALEX_H
#include "GoalEx.h"
#endif
#ifndef GAMESTATEEX_H
#include "GameStateEx.h"
#endif
#ifndef MATHHELPER_H
#include "MathHelper.h"
#endif
#ifndef CASEBASEEX_H
#include "CaseBaseEx.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif
#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif
#ifndef GOALFACTORY_H
#include "GoalFactory.h"
#endif
#include "AbstractRetainer.h"
#include "CaseBaseEx.h"
#include "Logger.h"
#include <map>
#include <functional>

using namespace std;
using namespace IStrategizer;
using namespace Serialization;

// The number of samples the KNN algorithm should use to calculate the most similar case to the
// case to be retrieved
const size_t KNeighbours = 4;

RetrieverEx::RetrieverEx(AbstractRetainer *p_pRetainer) : AbstractRetriever(p_pRetainer, "Retriever")
{
}
//----------------------------------------------------------------------------------------------
void RetrieverEx::BuildCaseCluster() 
{
    for(vector<CaseEx*>::iterator itr = m_pRetainer->CaseBase()->CaseContainer.begin();
        itr != m_pRetainer->CaseBase()->CaseContainer.end();
        ++itr)
    {
        vector<CaseEx*>& cluster = _caseCluster[(GoalType)(*itr)->Goal()->StepTypeId()];
        cluster.push_back(*itr);
    }
}
//----------------------------------------------------------------------------------------------
float RetrieverEx::GoalSimilarity(const GoalEx* p_g1, const GoalEx* p_g2)
{
    // returns a value between 0 and 1.0, where 1.0 means identical and 0.0 means totally different
    // GOAL-SIMILARITY GS(g1, g2)
    //  if g1 != g2
    //      return 0
    //  else
    //      return  alpha + (1 - alpha)PS(g1, g2))
    //
    // returns a value between 0 and 1.0, where 1 means identical and 0 means totally different
    // PARAMETER-SIMILARITY PS(g1, g2)
    //  foreach param p1 in g1 and param p2 in g2
    //      similarity += p1 == p2
    //  return similarity / paramsCount
    //
    if(p_g1->StepTypeId() != p_g2->StepTypeId())
    {
        return 0;
    }
    else
    {
        float alpha         = 0.5;
        float similarity    = 0;
        const PlanStepParameters& paramsG1 = p_g1->Parameters();
        const PlanStepParameters& paramsG2 = p_g2->Parameters();

        for(PlanStepParameters::const_iterator itrG1 = paramsG1.begin(), itrG2 = paramsG2.begin();
            itrG1 != p_g1->Parameters().end();
            ++itrG1, ++itrG2)
        {
            similarity += (itrG1->second == itrG2->second);
        }

        similarity /= (float)paramsG1.size();
        similarity *= (1 - alpha);
        similarity += alpha;

        return similarity;
    }
}
//----------------------------------------------------------------------------------------------
float RetrieverEx::StateSimilarity(const GameStateEx *p_gs1, const GameStateEx *p_gs2)
{
    // STATE-SIMILARITY(gs1, gs2)
    // distance = 0
    // for i: 0 to d
    // // distance += ((gs1[i] - gs2[i]) /  (1 + max(gs1[i], gs2[i]))) ^ 2
    // 
    // return 1 / (sqrt(distance) + 1.0)
    float distance = 0;
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

    return 1.0f / (sqrt(distance + 1.0f));
}
//----------------------------------------------------------------------------------------------
float RetrieverEx::CaseRelevance(const CaseEx* p_case, const GoalEx* p_goal, const GameStateEx* p_gameState)
{
    float alpha = 0.95f;
    float goalSimilarity = GoalSimilarity(p_case->Goal(), p_goal);
    // To Do: fix to use RtsGame somehow instead of GameStateEx
    float stateSimilarity = 0.0; //StateSimilarity(p_case->GameState(), p_gameState);

    return (alpha * goalSimilarity) + (float)((1.0 - alpha) * stateSimilarity);
}
//----------------------------------------------------------------------------------------------
CaseEx* RetrieverEx::Retrieve(const GoalEx* pGoal, const GameStateEx* pGameState, const set<CaseEx*>& exclusion)
{
    SVector<CaseEx*>& cases = m_pRetainer->CaseBase()->CaseContainer;
    multimap<float, CaseEx*, greater<float> > caseRelevanceTable;
    float caseRelevance;

    LogInfo("Retrieving case for goal={%s} and current game-state", pGoal->ToString().c_str());

    if (cases.empty())
    {
        LogError("Case-base is empty, retrieval failed");
        return nullptr;
    }

    // Calculate the relevance between each case in the case-base and
    // the current situation using the goal and game-state params
    for(size_t i = 0, size = cases.size(); i < size; ++i)
    {
        // Ignore all cases with different goal type because it does not
        // make sense to retrieve a WinGame case for TrainArmy goal for example
        if (pGoal->StepTypeId() != cases[i]->Goal()->StepTypeId())
            continue;

        if (exclusion.count(cases[i]) > 0)
        {
            LogInfo("Case %s is excluded from retrieval", cases[i]->Goal()->ToString().c_str());
            continue;
        }

        caseRelevance = CaseRelevance(cases[i], pGoal, pGameState);
        caseRelevanceTable.insert(make_pair(caseRelevance, cases[i]));
    }

    if (caseRelevanceTable.empty())
    {
        LogInfo("Failed to retrieve any matching case for %s", pGoal->ToString().c_str());
        return nullptr;
    }

    int i = 0;
    float outcome;
    float currCasePerformance;
    CaseEx* currCase;
    CaseEx* bestCase = nullptr;
    float bestCasePerformance = 0.0f;

    for(multimap<float, CaseEx*, greater<float> >::iterator itr = caseRelevanceTable.begin();
        itr != caseRelevanceTable.end() && i < 5;
        ++itr, ++i)
    {
        currCase = itr->second;
        outcome = (float)currCase->SuccessCount() / (float)currCase->TrialCount();
        _ASSERTE(outcome != 0);
        currCasePerformance = itr->first * outcome;

        if (currCasePerformance > bestCasePerformance)
        {
            bestCasePerformance = currCasePerformance;
            bestCase = currCase;
        }
    }

    LogInfo("Retrieved case '%s' with max performance=%f",
        bestCase->Goal()->ToString().c_str(),
        bestCasePerformance);

    return bestCase;
}
//----------------------------------------------------------------------------------------------
void RetrieverEx::ExecuteCommand(const char* p_cmd)
{
    //if(!strcmp(p_cmd, "retrieve"))
    //{
    //    PlanStepParameters m_parameters;
    //    m_parameters[PARAM_PlayerId] = PLAYER_Self;
    //    m_parameters[PARAM_StrategyTypeId] = STRTYPE_EarlyTierRush;

    //    CaseEx* myCase = Retrieve(g_GoalFactory.GetGoal(GOALEX_WinGame, m_parameters), g_Game->Self()->State());
    //}
}

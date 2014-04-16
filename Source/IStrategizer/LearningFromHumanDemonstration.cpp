#include "LearningFromHumanDemonstration.h"
#include "CaseBaseEx.h"
#include "ActionFactory.h"
#include "CaseLearningHelper.h"
#include "Diagraph.h"
#include "Action.h"
#include "GoalFactory.h"
#include "GoalEx.h"
#include "ConditionEx.h"

#ifndef RETAINEREX_H
#include "RetainerEx.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif

#include <queue>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;
using namespace IStrategizer;

LearningFromHumanDemonstration::LearningFromHumanDemonstration(PlayerType p_player, PlayerType p_enemy)
{
    _helper = new CaseLearningHelper();
    _retainer = new RetainerEx(g_CaseBasePath);
}
//------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::Learn()
{
    vector<CookedPlan*> m_cookedPlans;

    for each(auto m_rawCase in LearnRawCases(_helper->ObservedTraces()))
    {
        IStrategizer::CookedCase* m_cookedCase = DependencyGraphGeneration(m_rawCase);

        UnnecessaryStepsElimination(m_cookedCase);

        m_cookedPlans.push_back(new CookedPlan(
            m_cookedCase->rawCase->rawPlan.Goal,
            m_cookedCase->plan,
            m_cookedCase->rawCase->gameState));
    }

    for (size_t i = 0; i < m_cookedPlans.size(); ++i)
    {
        HierarchicalComposition(m_cookedPlans[i], m_cookedPlans, i);
    }

    RetainLearntCases(m_cookedPlans);
}
//------------------------------------------------------------------------------------------------
vector<RawCaseEx*> LearningFromHumanDemonstration::LearnRawCases(GameTrace::List traces)
{
    vector<RawCaseEx*> learntRawCases;
    vector<RawCaseEx*> candidateRawCases;
    CaseLearningHelper::GoalMatrix goalMatrix = _helper->GetGoalSatisfacionMatrix();

    // Learn the succeeded goals
    for each(auto goalPair in goalMatrix)
    {
        SequentialPlan plan;

        for(size_t i = 0; i < traces.size() && goalPair.first >= traces[i].GameCycle(); ++i)
        {
            // Set the action id to use it in the future to reference the trace game state.
            Action* action = g_ActionFactory.GetAction(traces[i].Action(), traces[i].ActionParams());
            action->Id(i);

            plan.push_back(action);
        }

        for (size_t j = 0; j < goalPair.second.size(); ++j)
        {
            candidateRawCases.push_back(new RawCaseEx(RawPlanEx(goalPair.second[j], plan), nullptr));
        }
    }

    // Remove duplicate cases
    for (size_t i = 0; i < candidateRawCases.size(); ++i)
    {
        bool duplicate = false;

        for (size_t j = 0; j < learntRawCases.size(); ++j)
        {
            if (candidateRawCases[i]->rawPlan.Goal == learntRawCases[j]->rawPlan.Goal &&
                IdenticalSequentialPlan(candidateRawCases[i]->rawPlan.sPlan, learntRawCases[j]->rawPlan.sPlan))
            {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
            learntRawCases.push_back(candidateRawCases[i]);
    }

    return learntRawCases;
}
//------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::IdenticalSequentialPlan(SequentialPlan left, SequentialPlan right)
{
    bool identical = left.size() == right.size();
    size_t i = 0;

    while (identical && i < left.size())
    {
        identical = left[i]->Id() == right[i++]->Id();
    }

    return identical;
}
//------------------------------------------------------------------------------------------------
CookedCase* LearningFromHumanDemonstration::DependencyGraphGeneration(RawCaseEx* p_rawCase)
{
    SequentialPlan m_planSteps;
    vector<Expression*> m_matchedConditions;
    OlcbpPlan* m_olcpbPlan = new OlcbpPlan();

    for (size_t i = 0; i < p_rawCase->rawPlan.sPlan.size(); ++i)
    {
        m_olcpbPlan->AddNode(p_rawCase->rawPlan.sPlan[i]);
        m_planSteps.push_back((PlanStepEx*)p_rawCase->rawPlan.sPlan[i]->Clone());
    }

    for (size_t i = 0; i < m_planSteps.size(); ++i)
    {
        for (size_t j = 0; j < m_planSteps.size(); ++j)
        {
            if (i != j)
            {
                if (Depends(m_planSteps[i]->PostCondition(), ((Action*)m_planSteps[j])->PreCondition(), m_matchedConditions))
                {
                    m_olcpbPlan->AddEdge(i, j);
                    m_matchedConditions.clear();
                }
            }
        }
    }

    return new CookedCase(p_rawCase, m_olcpbPlan);
}
//--------------------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::Depends(CompositeExpression* p_candidateNode, CompositeExpression* p_dependentNode, vector<Expression*>& p_matchedConditions)
{
    if (p_candidateNode && p_dependentNode)
    {
        vector< pair<Expression*,Expression*> > m_candidateConditions;
        ConditionEx* m_precondition;
        ConditionEx* m_postCondition;

        p_candidateNode->PartiallyEquals(p_dependentNode, m_candidateConditions);
        for (size_t i = 0; i < m_candidateConditions.size(); ++i)
        {
            m_precondition = (ConditionEx*)m_candidateConditions[i].second;
            m_postCondition = (ConditionEx*)m_candidateConditions[i].first;

            if (m_postCondition->Consume(m_precondition->ContainsParameter(PARAM_Amount) ? m_precondition->Parameter(PARAM_Amount) : 0))
            {
                p_matchedConditions.push_back(m_candidateConditions[i].first);
                p_dependentNode->RemoveExpression(m_candidateConditions[i].second);
            }
        }

        return (!p_matchedConditions.empty());
    }
    else
    {
        return false;
    }
}
//------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::UnnecessaryStepsElimination(CookedCase* p_case)
{
    SequentialPlan steps = vector<PlanStepEx*>(p_case->rawCase->rawPlan.sPlan.size());
    SequentialPlan fSteps;
    vector<int> rSteps;
    vector<Expression*> m_usedConditions;

    for (size_t i = 0; i < p_case->rawCase->rawPlan.sPlan.size(); ++i)
    {
        steps[i] = static_cast<PlanStepEx*>(p_case->rawCase->rawPlan.sPlan[i]->Clone());
    }

    // Extract direct steps
    for (size_t i = 0; i < steps.size(); ++i)
    {
        if (Depends(steps[i]->PostCondition(), p_case->rawCase->rawPlan.Goal->PostCondition(), m_usedConditions))
        {
            fSteps.push_back(steps[i]);
            rSteps.push_back(i);
            m_usedConditions.clear();
        }
    }

    size_t initSize = rSteps.size();
    steps.clear();

    for (size_t i = 0; i < p_case->rawCase->rawPlan.sPlan.size(); ++i)
    {
        steps.push_back(static_cast<PlanStepEx*>(p_case->rawCase->rawPlan.sPlan[i]->Clone()));
    }

    // Extract indirect steps
    for (size_t i = 0; i < initSize; ++i)
    {
        NecessaryStepsExtraction(steps, rSteps, fSteps);
    }

    p_case->rawCase->rawPlan.sPlan.clear();
    p_case->rawCase->rawPlan.sPlan.insert(p_case->rawCase->rawPlan.sPlan.begin(), fSteps.begin(), fSteps.end());

    if(!fSteps.empty())
    {
        // use the game state of the first action, the index of the trace is stored in the data storage
        GameTrace firstTrace = _helper->ObservedTraces()[fSteps[fSteps.size() - 1]->Id()];
        p_case->rawCase->gameState = firstTrace.GameState();
    }

    p_case->plan = new OlcbpPlan();

    for(size_t i = 0; i < fSteps.size(); ++i)
    {
        p_case->plan->AddNode(fSteps[i]);
    }
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::NecessaryStepsExtraction(const SequentialPlan& p_steps, vector<int>& p_rSteps, SequentialPlan& p_fSteps)
{
    vector<Expression*> m_usedConditions;

    for (size_t i = 0; i < p_rSteps.size(); ++i)
    {
        for (size_t j = 0; j < p_steps.size(); ++j)
        {
            if (Depends(p_steps[j]->PostCondition(), ((Action*)p_steps[p_rSteps[i]])->PreCondition(), m_usedConditions))
            {
                p_fSteps.push_back(p_steps[j]);
                p_rSteps.push_back(j);
                m_usedConditions.clear();
            }
        }

        p_rSteps.erase(p_rSteps.begin() + i);
    }

    for (size_t i = 0; i < p_rSteps.size(); ++i)
    {
        NecessaryStepsExtraction(p_steps, p_rSteps, p_fSteps);
    }
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::HierarchicalComposition(CookedPlan* p_plan, const vector<CookedPlan*>& p_plans, unsigned p_index)
{
    int m_maxSubgraphIndex = INT_MAX;
    int m_maxMatchedCount = 0;
    int m_currentMatchedCount;
    vector<int> m_cMatchedIndexes;
    vector<int> m_mMatchedIndexes;

    for (size_t i = 0; i < p_plans.size(); ++i)
    {
        if (i != p_index && p_plan->pPlan->IsSubGraph((*p_plans[i]->pPlan), m_cMatchedIndexes, m_currentMatchedCount))
        {
            if (m_currentMatchedCount > m_maxMatchedCount)
            {
                m_maxSubgraphIndex = i;
                m_mMatchedIndexes = m_cMatchedIndexes;
                m_maxMatchedCount = m_currentMatchedCount;
            }
        }
    }

    if (m_maxSubgraphIndex != INT_MAX)
    {
        p_plan->pPlan->SubGraphSubstitution(m_mMatchedIndexes, p_plans[m_maxSubgraphIndex]->Goal);
    }
}
//----------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::RetainLearntCases(vector<CookedPlan*>& p_cookedPlans)
{
    CaseEx* pLearntCase = nullptr;

    _retainer->ReadCaseBase();

    for(size_t i = 0, size = p_cookedPlans.size(); i < size; ++i)
    {
        CookedPlan* currCookedPlan = p_cookedPlans[i];
        pLearntCase = new CaseEx(currCookedPlan->pPlan, currCookedPlan->Goal, currCookedPlan->gameState, 1, 1);
        _retainer->Retain(pLearntCase);
    }

    _retainer->Flush();
}
//--------------------------------------------------------------------------------------------------------------
LearningFromHumanDemonstration::~LearningFromHumanDemonstration()
{
    Toolbox::MemoryClean(_helper);
    Toolbox::MemoryClean(_retainer);
}

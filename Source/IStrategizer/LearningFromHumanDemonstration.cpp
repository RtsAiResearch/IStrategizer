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
    vector<RawCaseEx*> m_rawCases = LearnRawCases(_helper->ObservedTraces());

    vector<CookedCase*> m_cookedCases;
    vector<PlanStepEx*> m_steps;

    m_cookedCases.resize(m_rawCases.size());

    for (size_t i = 0; i < m_rawCases.size(); ++i)
    {
        m_cookedCases[i] = DependencyGraphGeneration(m_rawCases[i]);
    }

    for (size_t i = 0; i < m_cookedCases.size(); ++i)
    {
        UnnecessaryStepsElimination(m_cookedCases[i]);
    }

    for (size_t i = 0; i < m_cookedCases.size(); ++i)
    {
        m_cookedCases[i] = DependencyGraphGeneration(m_cookedCases[i]->rawCase);
    }

    for (size_t i = 0; i < m_cookedCases.size(); ++i)
    {
        m_cookedPlans.push_back( new CookedPlan(m_cookedCases[i]->rawCase->rawPlan.Goal, m_cookedCases[i]->plan, m_cookedCases[i]->rawCase->gameState));
    }

    // Initial code of adding plans to High Level Steps using Medium Level
    // FIXME: needs to be recursive.
    // for (size_t i = 0; i < m_cookedPlans.size(); ++i)
    // {
    //     if (m_cookedPlans[i]->Goal->LevelType() == SLEVELTYPE_High)
    //     {
    //         vector<Expression*> m_usedConditions;
    //         for (size_t j = m_cookedPlans.size() - 1; j > 0; --j)
    //         {
    //             if(Depends(m_cookedPlans[j]->Goal->PostConditions(), m_cookedPlans[i]->Goal->SuccessConditions(), m_usedConditions))
    //             {
    //                 m_cookedPlans[i]->pPlan = new OlcbpPlan(m_cookedPlans[j]->Goal);
    //                 break;
    //             }
    //         }
    //     }
    // }

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
    for (CaseLearningHelper::GoalMatrix::iterator itr = goalMatrix.begin(); itr != goalMatrix.end(); itr++)
    {
        size_t i = 0;
        SequentialPlan plan;

        do
        {
            // Set the action id to use it in the future to reference the trace game state.
            Action* action = g_ActionFactory.GetAction(traces[i].Action(), traces[i].ActionParams());
            action->Id(i);

            plan.push_back(action);
        }
        while ((*itr).first >= traces[++i].GameCycle());

        vector<GoalEx*> currentGoals = (*itr).second;

        for (size_t j = 0; j < currentGoals.size(); ++j)
        {
            candidateRawCases.push_back(new RawCaseEx(RawPlanEx(currentGoals[j], plan), nullptr));
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
void LearningFromHumanDemonstration::AddAction(RawCaseEx* p_case, ActionType p_action, const PlanStepParameters& p_params, int p_traceId)
{
    Action* action = g_ActionFactory.GetAction(p_action, p_params);
    action->Data(p_traceId);
    action->InitializeConditions();

    p_case->rawPlan.sPlan.push_back(action);
}
//------------------------------------------------------------------------------------------------
CookedCase* LearningFromHumanDemonstration::DependencyGraphGeneration(RawCaseEx* p_rawCase)
{
    SequentialPlan m_planSteps = vector<PlanStepEx*>(p_rawCase->rawPlan.sPlan.size());
    unsigned m_stepsCount = m_planSteps.size();
    vector<Expression*> m_matchedConditions;
    OlcbpPlan* m_olcpbPlan = new OlcbpPlan();

    for (size_t i = 0; i < p_rawCase->rawPlan.sPlan.size(); ++i)
    {
        PlanStepEx* planStep = static_cast<PlanStepEx*>(p_rawCase->rawPlan.sPlan[i]->Clone());
        m_olcpbPlan->AddNode(planStep);
    }

    for (size_t i = 0; i < m_stepsCount; ++i)
    {
        for (size_t j = 0; j < m_stepsCount; ++j)
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

            if (m_postCondition->Consume( (m_precondition->ContainsParameter(PARAM_Amount)) ? m_precondition->Parameter(PARAM_Amount) : 0 ))
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
        NecessaryStepsExtraction(p_case->plan, rSteps[i], fSteps, steps);
    }

    p_case->rawCase->rawPlan.sPlan.clear();
    p_case->rawCase->rawPlan.sPlan.insert(p_case->rawCase->rawPlan.sPlan.begin(), fSteps.begin(), fSteps.end());

    if(!fSteps.empty())
    {
        // use the game state of the first action, the index of the trace is stored in the data storage
        GameTrace firstTrace = _helper->ObservedTraces()[fSteps[0]->Data()];
        p_case->rawCase->gameState = firstTrace.GameState();
    }

    p_case->plan = new OlcbpPlan();

    for(size_t i = 0; i < fSteps.size(); ++i)
    {
        p_case->plan->AddNode(fSteps[i]);
    }
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::NecessaryStepsExtraction(OlcbpPlan* p_plan, unsigned p_sIndex, SequentialPlan& p_fSteps, const SequentialPlan& p_steps)
{
    vector< pair<int, PlanStepEx*> > rSteps;
    vector<int> toDelete;

    for (size_t i = 0; i < p_plan->Size(); ++i)
    {
        if (i != p_sIndex && p_plan->IsAdjacent(i, p_sIndex))
        {
            if (find(p_fSteps.begin(), p_fSteps.end(), p_steps[i]) == p_fSteps.end())
            {
                rSteps.push_back(make_pair(i, p_steps[i]));
                p_fSteps.insert(p_fSteps.begin(), p_steps[i]);
            }
        }
    }

    for (size_t i = 0; i < rSteps.size(); ++i)
    {
        NecessaryStepsExtraction(p_plan, rSteps[i].first, p_fSteps, p_steps);
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
void LearningFromHumanDemonstration::RetainLearntCases( vector<CookedPlan*>& p_cookedPlans )
{
    CaseEx* pLearntCase = nullptr;

    _retainer->ReadCaseBase();

    for(size_t i = 0, size = p_cookedPlans.size(); i < size; ++i)
    {
        CookedPlan* currCookedPlan = p_cookedPlans[i];
        pLearntCase = new CaseEx(NULL, currCookedPlan->Goal, currCookedPlan->gameState, 1, 1);
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

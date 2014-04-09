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
    
 //   vector<CookedCase*> m_cookedCases;
    //vector<PlanStepEx*> m_steps;

 //   m_cookedCases.resize(m_rawCases.size());

 //   for (int i = 0; i < m_rawCases.size(); ++i)
 //   {
 //       m_cookedCases[i] = DependencyGraphGeneration(m_rawCases[i]);
 //   }

 //   for (int i = 0; i < m_cookedCases.size(); ++i)
 //   {
 //       UnnecessaryStepsElimination(m_cookedCases[i]);
 //   }

 //   for (int i = 0; i < m_cookedCases.size(); ++i)
 //   {
 //       m_cookedCases[i] = DependencyGraphGeneration(m_cookedCases[i]->rawCase);
 //   }

 //   for (int i = 0; i < m_cookedCases.size(); ++i)
 //   {
    // m_cookedPlans.push_back( new CookedPlan(m_cookedCases[i]->rawCase->rawPlan.Goal, m_cookedCases[i]->dGraph, m_cookedCases[i]->rawCase->gameState));
 //   }

    //// Initial code of adding plans to High Level Steps using Medium Level
    //// FIXME: needs to be recursive.
    //for (int i = 0; i < m_cookedPlans.size(); ++i)
    //{
    // if (m_cookedPlans[i]->Goal->LevelType() == SLEVELTYPE_High)
    // {
    // vector<Expression*> m_usedConditions;
    // for (int j = m_cookedPlans.size() - 1; j > 0; --j)
    // {
    // if(Depends(m_cookedPlans[j]->Goal->PostConditions(), m_cookedPlans[i]->Goal->SuccessConditions(), m_usedConditions))
    // {
    // m_cookedPlans[i]->pPlan = new PlanGraph(m_cookedPlans[j]->Goal);
    // break;
    // }
    // }
    // }
    //}

    //for (int i = 0; i < m_cookedPlans.size(); ++i)
    //{
    // HierarchicalComposition(m_cookedPlans[i], m_cookedPlans, i);
    //}

    RetainLearntCases(m_cookedPlans);
}
//------------------------------------------------------------------------------------------------
vector<RawCaseEx*> LearningFromHumanDemonstration::LearnRawCases(GameTrace::List p_traces)
{
    size_t rowSize = _helper->GetGoalMatrixRowEvaluator().GetRowSize();
    vector<RawCaseEx*> learntRawCases;
    vector<RawCaseEx*> currentCases = vector<RawCaseEx*>(rowSize, nullptr);
    RawPlanEx tempRPlan;
    size_t i, g;

    for (i = 0; i < p_traces.size(); ++i)
    {
        for (g = 0; g < rowSize; ++g)
        {
            const GoalMatrixRow& currGoalMatrixRow = _helper->GetGoalMatrixRow(p_traces[i]);
            
            // If the goal in index g is not satisfied and is has no acions added, add it to the current cases.
            if (!currGoalMatrixRow[g] && !currentCases[g])
            {
                tempRPlan = RawPlanEx(nullptr, SequentialPlan());
                currentCases[g] = new RawCaseEx(tempRPlan, p_traces[i].GameState());
                AddAction(currentCases[g], p_traces[i].Action(), p_traces[i].ActionParams(), i);
            }
            // If the goal in index g is not satisfied and has actions added, add new action to its current sequential plan.
            else if(!currGoalMatrixRow[g] && currentCases[g])
            {
                AddAction(currentCases[g], p_traces[i].Action(), p_traces[i].ActionParams(), i);
            }
            // If the goal is satisfied then add it to learnt cases!
            else if (currGoalMatrixRow[g] && currentCases[g])
            {
                currentCases[g]->rawPlan.Goal = _helper->GetGoalMatrixRowEvaluator().GetGoal(g);
                AddAction(currentCases[g], p_traces[i].Action(), p_traces[i].ActionParams(), i);
                learntRawCases.push_back(currentCases[g]);
                currentCases[g] = nullptr;
            }
        }
    }

    return learntRawCases;
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
    PlanGraph* m_graph;
    Action *aJ;
    unsigned i, j;
    vector<Expression*> m_matchedConditions;

    for ( i = 0; i < p_rawCase->rawPlan.sPlan.size(); ++i)
    {
        m_planSteps[i] = static_cast<PlanStepEx*>(p_rawCase->rawPlan.sPlan[i]->Clone());
    }

    m_graph = new PlanGraph(m_planSteps);

    for (i = 0; i < m_stepsCount; ++i)
    {
        for (j = 0; j < m_stepsCount; ++j)
        {
            if (i != j)
            {
                aJ = (Action*)m_planSteps[j];

                if (Depends(m_planSteps[i]->PostCondition(), aJ->PreCondition(), m_matchedConditions))
                {
                    m_graph->Connect(i, j, m_matchedConditions);
                    m_matchedConditions.clear();
                }
            }
        }
    }

    return new CookedCase(p_rawCase, m_graph);
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
        for (unsigned i = 0; i < m_candidateConditions.size(); ++i)
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
    unsigned int i, initSize;
    vector<Expression*> m_usedConditions;

    for ( i = 0; i < p_case->rawCase->rawPlan.sPlan.size(); ++i)
    {
        steps[i] = static_cast<PlanStepEx*>(p_case->rawCase->rawPlan.sPlan[i]->Clone());
    }

    // Extract direct steps
    for (i = 0; i < steps.size(); ++i)
    {
        if (Depends(steps[i]->PostCondition(), p_case->rawCase->rawPlan.Goal->PostCondition(), m_usedConditions))
        {
            fSteps.push_back(steps[i]);
            rSteps.push_back(i);
            m_usedConditions.clear();
        }
    }

    initSize = rSteps.size();
    steps.clear();

    for ( i = 0; i < p_case->rawCase->rawPlan.sPlan.size(); ++i)
    {
        steps.push_back(static_cast<PlanStepEx*>(p_case->rawCase->rawPlan.sPlan[i]->Clone()));
    }

    // Extract indirect steps
    for (i = 0; i < initSize; ++i)
    {
        NecessaryStepsExtraction(p_case->dGraph, rSteps[i], fSteps, steps);
    }

    p_case->rawCase->rawPlan.sPlan.clear();
    p_case->rawCase->rawPlan.sPlan.insert(p_case->rawCase->rawPlan.sPlan.begin(), fSteps.begin(), fSteps.end());

    if(!fSteps.empty())
    {
        // use the game state of the first action, the index of the trace is stored in the data storage
        GameTrace firstTrace = _helper->ObservedTraces()[fSteps[0]->Data()];
        p_case->rawCase->gameState = firstTrace.GameState();
    }

    p_case->dGraph = new PlanGraph(fSteps);
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::NecessaryStepsExtraction(PlanGraph* p_graph, unsigned p_sIndex, SequentialPlan& p_fSteps, const SequentialPlan& p_steps)
{
    vector< pair<int, PlanStepEx*> > rSteps;
    unsigned i;
    vector<int> toDelete;

    for (i = 0; i < p_graph->Size(); ++i)
    {
        if (i != p_sIndex && p_graph->IsConnected(i, p_sIndex))
        {
            if (find(p_fSteps.begin(), p_fSteps.end(), p_steps[i]) == p_fSteps.end())
            {
                rSteps.push_back(make_pair(i, p_steps[i]));
                p_fSteps.insert(p_fSteps.begin(), p_steps[i]);
            }
        }
    }

    for (i = 0; i < rSteps.size(); ++i)
    {
        NecessaryStepsExtraction(p_graph, rSteps[i].first, p_fSteps, p_steps);
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
    
    for (unsigned i = 0; i < p_plans.size(); ++i)
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
        pLearntCase = new CaseEx(nullptr/*currCookedPlan->pPlan*/, currCookedPlan->Goal, currCookedPlan->gameState, 1, 1);
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

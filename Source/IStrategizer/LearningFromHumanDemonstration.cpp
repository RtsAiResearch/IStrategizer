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

    for each(auto m_rawCase in m_rawCases)
    {
        IStrategizer::CookedCase* m_cookedCase = DependencyGraphGeneration(m_rawCase);
        
        UnnecessaryStepsElimination(m_cookedCase);
        
        m_cookedPlans.push_back(new CookedPlan(
            m_cookedCase->rawCase->rawPlan.Goal,
            m_cookedCase->plan,
            m_cookedCase->rawCase->gameState));
    }

    HierarchicalComposition(m_cookedPlans);
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
    OlcbpPlan* m_olcpbPlan = new OlcbpPlan();

    for (size_t i = 0; i < p_rawCase->rawPlan.sPlan.size(); ++i)
    {
        m_olcpbPlan->AddNode((PlanStepEx*)p_rawCase->rawPlan.sPlan[i]->Clone());
    }

    for each(size_t i in m_olcpbPlan->GetNodes())
    {
        for each(size_t j in m_olcpbPlan->GetNodes())
        {
            if(i != j)
            {
                if(Depends(m_olcpbPlan->GetNode(i)->PostCondition(), ((Action*)m_olcpbPlan->GetNode(j))->PreCondition()))
                {
                    m_olcpbPlan->AddEdge(i, j);
                }
            }
        }
    }

    return new CookedCase(p_rawCase, m_olcpbPlan);
}
//--------------------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::Depends(CompositeExpression* p_candidateNode, CompositeExpression* p_dependentNode)
{
    assert(p_candidateNode);
    assert(p_dependentNode);

    vector< pair<Expression*,Expression*> > m_candidateConditions;
    p_candidateNode->PartiallyEquals(p_dependentNode, m_candidateConditions);

    for each(auto m_candidateCondition in m_candidateConditions)
    {
        ConditionEx* m_precondition = (ConditionEx*)m_candidateCondition.second;
        ConditionEx* m_postCondition = (ConditionEx*)m_candidateCondition.first;

        if (m_postCondition->Consume(m_precondition->ContainsParameter(PARAM_Amount) ? m_precondition->Parameter(PARAM_Amount) : 0))
        {
            return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::UnnecessaryStepsElimination(CookedCase* p_case)
{
    OlcbpPlan::NodeSet m_unprocessedSteps = p_case->plan->GetNodes();
    OlcbpPlan::NodeSet m_necessarySteps;
    OlcbpPlan::NodeSet m_finalSteps;

    for(OlcbpPlan::NodeSet::iterator it = m_unprocessedSteps.begin(); it != m_unprocessedSteps.end();)
    {
        if (Depends(p_case->plan->GetNode(*it)->PostCondition(), p_case->rawCase->rawPlan.Goal->PostCondition()))
        {
            m_necessarySteps.insert(*it);
            m_unprocessedSteps.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    while(m_necessarySteps.size())
    {
        OlcbpPlan::NodeID current = *m_necessarySteps.begin();
        m_necessarySteps.erase(current);
        m_finalSteps.insert(current);

        for(OlcbpPlan::NodeSet::iterator it = m_unprocessedSteps.begin(); it != m_unprocessedSteps.end();)
        {
            if(p_case->plan->IsAdjacent(*it, current))
            {
                m_necessarySteps.insert(*it);
                m_unprocessedSteps.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }

    for each(OlcbpPlan::NodeID i in p_case->plan->GetNodes())
    {
        if(m_finalSteps.find(i) == m_finalSteps.end())
        {
            p_case->plan->RemoveNode(i);
        }
    }
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::HierarchicalComposition(std::vector<CookedPlan*>& p_cookedPlans)
{
    for (size_t i = 0; i < p_cookedPlans.size(); ++i)
    {
        for (size_t j = 0; j < p_cookedPlans.size(); ++j)
        {
            if (i != j)
            {
                std::vector<int> m_matchedIndexes;

                if (p_cookedPlans[i]->pPlan->IsSubGraphOf((*p_cookedPlans[j]->pPlan), m_matchedIndexes))
                {
                    p_cookedPlans[j]->pPlan->SubGraphSubstitution(m_matchedIndexes, p_cookedPlans[i]->Goal);
                }
            }
        }
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
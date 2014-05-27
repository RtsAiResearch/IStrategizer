#include "LearningFromHumanDemonstration.h"
#include "CaseBaseEx.h"
#include "ActionFactory.h"
#include "CaseLearningHelper.h"
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
    int count = 0;
    for (auto m_rawCase : m_rawCases)
    {
        IStrategizer::CookedCase* m_cookedCase = DependencyGraphGeneration(m_rawCase);
        LogInfo("The initial plan for case %d/%d for goal %s is:\n%s",
            count,
            m_rawCases.size(),
            m_cookedCase->rawCase->rawPlan.Goal->ToString(true).c_str(),
            m_cookedCase->plan->ToString().c_str());
        
        UnnecessaryStepsElimination(m_cookedCase);

        if (m_cookedCase->plan->Size() > 0)
        {
            m_cookedPlans.push_back(new CookedPlan(
                m_cookedCase->rawCase->rawPlan.Goal,
                m_cookedCase->plan,
                m_cookedCase->rawCase->gameState));
            
            LogInfo("Finished learning case %d/%d for goal %s. The final plan is:\n%s",
                count++,
                m_rawCases.size(),
                m_cookedCase->rawCase->rawPlan.Goal->ToString(true).c_str(),
                m_cookedCase->plan->ToString().c_str());
        }
        else
        {
            LogInfo("Finished learning case %d/%d and the case does not have any steps so it will not be included.", count++, m_rawCases.size());
        }
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
    SequentialPlan plan;
    unsigned currentTraceInx = 0;

    // Learn the succeeded goals
    for (auto goalPair : goalMatrix)
    {
        for (; currentTraceInx < traces.size() && goalPair.first >= traces[currentTraceInx].GameCycle(); ++currentTraceInx)
        {
            Action* action = g_ActionFactory.GetAction(traces[currentTraceInx].Action(), traces[currentTraceInx].ActionParams(), true);
            _ASSERTE(action->PostCondition());
            _ASSERTE(action->PreCondition());

            _gameStateMapping[action->Id()] = traces[currentTraceInx].GameState();
            plan.push_back(action);
        }

        for (unsigned j = 0; j < goalPair.second.size(); ++j)
        {
            candidateRawCases.push_back(new RawCaseEx(RawPlanEx(goalPair.second[j], plan), nullptr));
        }
    }

    // Remove duplicate cases
    for (unsigned i = 0; i < candidateRawCases.size(); ++i)
    {
        bool duplicate = false;

        for (unsigned j = 0; j < learntRawCases.size(); ++j)
        {
            bool identicalGoal = candidateRawCases[i]->rawPlan.Goal->Equals(learntRawCases[j]->rawPlan.Goal);
            bool identicalPlan = IdenticalSequentialPlan(candidateRawCases[i]->rawPlan.sPlan, learntRawCases[j]->rawPlan.sPlan);
            
            if (identicalGoal && identicalPlan)
            {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
        {
            learntRawCases.push_back(candidateRawCases[i]);
        }
    }

    return learntRawCases;
}
//------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::IdenticalSequentialPlan(SequentialPlan left, SequentialPlan right)
{
    bool identical = left.size() == right.size();
    unsigned i = 0;

    while (identical && ++i < left.size())
    {
        identical = left[i]->Equals(right[i]);
    }

    return identical;
}
//------------------------------------------------------------------------------------------------
CookedCase* LearningFromHumanDemonstration::DependencyGraphGeneration(RawCaseEx* p_rawCase)
{
    OlcbpPlan* m_olcpbPlan = new OlcbpPlan();
    OlcbpPlan* m_tempOlcpbPlan = new OlcbpPlan();
    OlcbpPlan::NodeList nodeIds;

    for (unsigned i = 0; i < p_rawCase->rawPlan.sPlan.size(); ++i)
    {
        PlanStepEx* pPlanStep = p_rawCase->rawPlan.sPlan[i];
        PlanStepEx* pTempClone = (PlanStepEx*)pPlanStep->Clone();
        m_olcpbPlan->AddNode(pPlanStep, pPlanStep->Id());
        m_tempOlcpbPlan->AddNode(pTempClone, pPlanStep->Id());
        nodeIds.push_back(pPlanStep->Id());

        // Update the game state mapping with the new Id
        _gameStateMapping[pPlanStep->Id()] = _gameStateMapping[p_rawCase->rawPlan.sPlan[i]->Id()];
    }
    
    for (unsigned i = nodeIds.size(); i-- > 0; )
    {
        for (unsigned j = i + 1; j < nodeIds.size(); ++j)
        {
            _ASSERTE(j > i);
            OlcbpPlan::NodeID candidateParent = nodeIds[i];
            OlcbpPlan::NodeID candidateChild = nodeIds[j];

            LogInfo("Checking dependency between node %s and %s",
                m_olcpbPlan->GetNode(candidateParent)->ToString(true).c_str(),
                m_olcpbPlan->GetNode(candidateChild)->ToString(true).c_str());
            
            if (Depends(m_tempOlcpbPlan->GetNode(candidateParent)->PostCondition(), ((Action*)m_tempOlcpbPlan->GetNode(candidateChild))->PreCondition()))
            {
                LogInfo("Adding edge from node %s to node %s, as dependency matches",
                    m_olcpbPlan->GetNode(candidateParent)->ToString(true).c_str(),
                    m_olcpbPlan->GetNode(candidateChild)->ToString(true).c_str());
                
                m_olcpbPlan->AddEdge(candidateParent, candidateChild);
            }
        }
    }

    return new CookedCase(p_rawCase, m_olcpbPlan);
}
//--------------------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::Depends(CompositeExpression* p_candidateParent, CompositeExpression* p_candidateChild)
{
    assert(p_candidateParent);
    assert(p_candidateChild);

    vector<pair<Expression*,Expression*>> m_candidateConditions;
    p_candidateParent->PartiallyEquals(p_candidateChild, m_candidateConditions);

    for (auto m_candidateCondition : m_candidateConditions)
    {
        ConditionEx* consumer = (ConditionEx*)m_candidateCondition.second;
        ConditionEx* source = (ConditionEx*)m_candidateCondition.first;
        LogInfo("Found candidate matched condition %s", Enums[source->Type()]);
        bool preconditionHasAmount = consumer->ContainsParameter(PARAM_Amount);
        bool postconditionHasAmount = source->ContainsParameter(PARAM_Amount);
        _ASSERTE(preconditionHasAmount == postconditionHasAmount);
        if (preconditionHasAmount && postconditionHasAmount)
        {
            int requiredAmount = consumer->Parameter(PARAM_Amount);
            int availableAmount = source->Parameter(PARAM_Amount);
            int consumedAmount = min(requiredAmount, availableAmount);
            LogInfo("The consumer amount is %d and the source amount is %d", requiredAmount, availableAmount);
            
            if (source->Consume(requiredAmount))
            {
                _ASSERTE(consumer->Consume(consumedAmount));
                return true;
            }
        }
        else
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
    map<OlcbpPlan::NodeID, OlcbpPlan::NodeValue> m_tempNodes;
    CompositeExpression* goalPostConditionCopy = (CompositeExpression*)p_case->rawCase->rawPlan.Goal->PostCondition()->Clone();

    for (OlcbpPlan::NodeID nodeId : m_unprocessedSteps)
    {
        m_tempNodes[nodeId] = (PlanStepEx*)p_case->plan->GetNode(nodeId)->Clone();
    }

    LogInfo("Finding necessary steps for goal node %s", p_case->rawCase->rawPlan.Goal->ToString(true).c_str());
    for (OlcbpPlan::NodeSet::iterator it = m_unprocessedSteps.begin(); it != m_unprocessedSteps.end();)
    {
        // The order of depends is important keep goal on right side and action on left side
        // that's because if the goal depends on the action we'll consume the action's resources
        // and assign the goals requirements as well.
        if (Depends(m_tempNodes[(*it)]->PostCondition(), goalPostConditionCopy))
        {
            m_necessarySteps.insert(*it);

            // Set the game state to the latest dependent action in the plan
            p_case->rawCase->gameState = _gameStateMapping[p_case->plan->GetNode(*it)->Id()];

            m_unprocessedSteps.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    _ASSERTE(m_necessarySteps.size() > 0);

    while (m_necessarySteps.size())
    {
        OlcbpPlan::NodeID current = *m_necessarySteps.begin();
        m_necessarySteps.erase(current);
        m_finalSteps.insert(current);

        for (OlcbpPlan::NodeSet::iterator it = m_unprocessedSteps.begin(); it != m_unprocessedSteps.end();)
        {
            if (p_case->plan->IsAdjacent(*it, current))
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

    // Removing unnecessary steps
    for (OlcbpPlan::NodeID i : p_case->plan->GetNodes())
    {
        if (m_finalSteps.find(i) == m_finalSteps.end())
        {
            p_case->plan->RemoveNode(i);
        }
    }
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::HierarchicalComposition(std::vector<CookedPlan*>& p_cookedPlans)
{
    map<int, vector<int>> subplansIndex;
    map<int, vector<OlcbpPlan::NodeSet>> matchedSubplansNodeIds;
    OlcbpPlan::NodeSet totalMatchedNodeIds;

    for (unsigned i = 0; i < p_cookedPlans.size(); ++i)
    {
        LogInfo("Superset Plan Graph:\n%s", p_cookedPlans[i]->pPlan->ToString().c_str());
        
        totalMatchedNodeIds.clear();

        for (unsigned j = 0; j < p_cookedPlans.size(); ++j)
        {
            // Make sure not to match with the same plan.
            if (i != j)
            {
                OlcbpPlan::NodeSet matchedIds;
                LogInfo("Candidate subplan Plan Graph:\n%s", p_cookedPlans[j]->pPlan->ToString().c_str());

                if (p_cookedPlans[i]->pPlan->IsSuperGraphOf(p_cookedPlans[j]->pPlan, matchedIds))
                {
                    OlcbpPlan::NodeSet newMatchedNodeIds;
                    set_difference(matchedIds.begin(), matchedIds.end(), totalMatchedNodeIds.begin(), totalMatchedNodeIds.end(), inserter(newMatchedNodeIds, newMatchedNodeIds.end()));
                    totalMatchedNodeIds.insert(newMatchedNodeIds.begin(), newMatchedNodeIds.end());
                    if (!newMatchedNodeIds.empty())
                    {
                        matchedSubplansNodeIds[i].push_back(newMatchedNodeIds);
                        subplansIndex[i].push_back(j);
                    }
                }
            }
        }
    }

    for (unsigned i = 0; i < p_cookedPlans.size(); ++i)
    {
        if (!subplansIndex[i].empty())
        {
            int subplanMatchedIdsIndex = 0;
            for (int subplanIndex : subplansIndex[i])
            {
                LogInfo("First subplan Plan Graph:\n%s", p_cookedPlans[subplanIndex]->pPlan->ToString().c_str());
                p_cookedPlans[i]->pPlan->SubGraphSubstitution(
                    matchedSubplansNodeIds[i][subplanMatchedIdsIndex++],
                    p_cookedPlans[subplanIndex]->Goal,
                    p_cookedPlans[subplanIndex]->Goal->Id());
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::RetainLearntCases(vector<CookedPlan*>& p_cookedPlans)
{
    CaseEx* pLearntCase = nullptr;

    _retainer->ReadCaseBase();

    for (unsigned i = 0, size = p_cookedPlans.size(); i < size; ++i)
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
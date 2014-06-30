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
using namespace Serialization;

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
        
        UnnecessaryStepsElimination(m_cookedCase);

        if (m_cookedCase->plan->Size() > 0)
        {
            m_cookedPlans.push_back(new CookedPlan(
                m_cookedCase->rawCase->rawPlan.Goal,
                m_cookedCase->plan,
                m_cookedCase->rawCase->gameState));

            LogInfo("Finished learning case %d/%d", count++, m_rawCases.size());
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
bool LearningFromHumanDemonstration::IdenticalSequentialPlan(SequentialPlan left, SequentialPlan right) const
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

            if (!m_olcpbPlan->Reachable(candidateParent, candidateChild) &&
                Depends(m_tempOlcpbPlan->GetNode(candidateParent)->PostCondition(), ((Action*)m_tempOlcpbPlan->GetNode(candidateChild))->PreCondition()))
            {
                m_olcpbPlan->AddEdge(candidateParent, candidateChild);
            }
        }
    }

    return new CookedCase(p_rawCase, m_olcpbPlan);
}
//--------------------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::Depends(CompositeExpression* p_candidateParent, CompositeExpression* p_candidateChild) const
{
    assert(p_candidateParent);
    assert(p_candidateChild);

    vector<pair<Expression*,Expression*>> m_candidateConditions;
    p_candidateParent->PartiallyEquals(p_candidateChild, m_candidateConditions);

    if (m_candidateConditions.empty())
    {
        return false;
    }

    bool depends = false;
    
    for (auto m_candidateCondition : m_candidateConditions)
    {
        ConditionEx* consumer = (ConditionEx*)m_candidateCondition.second;
        ConditionEx* source = (ConditionEx*)m_candidateCondition.first;
        bool preconditionHasAmount = consumer->ContainsParameter(PARAM_Amount);
        bool postconditionHasAmount = source->ContainsParameter(PARAM_Amount);
        _ASSERTE(preconditionHasAmount == postconditionHasAmount);
        if (preconditionHasAmount && postconditionHasAmount)
        {
            int requiredAmount = consumer->Parameter(PARAM_Amount);
            int availableAmount = source->Parameter(PARAM_Amount);
            int consumedAmount = min(requiredAmount, availableAmount);
            
            if (source->Consume(consumedAmount))
            {
                _ASSERTE(consumer->Consume(consumedAmount));
                depends = true;
            }
        }
    }

    return depends;
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
void LearningFromHumanDemonstration::HierarchicalComposition(vector<CookedPlan*>& p_cookedPlans) const
{
    bool composePlans = false;

    do
    {
        IdenticalPlanDetection(p_cookedPlans);
        composePlans = SubplansDetection(p_cookedPlans);
    } while (composePlans);
}
//----------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::IsSuperGraph(OlcbpPlan *pSuperGraph, OlcbpPlan *pSubGraph, OlcbpPlan::NodeSet& superGraphMatchedIds) const
{
    if (pSuperGraph->Size() <= pSubGraph->Size() || pSubGraph->Size() == 1)
    {
        return false;
    }

    OlcbpPlan::NodeSet subgraphNodes = pSubGraph->GetNodes();
    OlcbpPlan::PlanHashMap superGraphPlanHash = pSuperGraph->PlanHash;
    
    for (OlcbpPlan::NodeID subGraphNodeId : subgraphNodes)
    {
        unsigned nodeHash = ((PlanStepEx*)pSubGraph->GetNode(subGraphNodeId))->Hash();
        if (!superGraphPlanHash[nodeHash].empty())
        {
            unsigned superGraphNodeId = *superGraphPlanHash[nodeHash].begin();
            _ASSERTE(pSuperGraph->GetNode(superGraphNodeId));
            superGraphMatchedIds.insert(superGraphNodeId);
            superGraphPlanHash[nodeHash].erase(superGraphNodeId);
        }
        else
        {
            return false;
        }
    }

    return true;
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
//--------------------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::IsIdenticalPlan(OlcbpPlan* leftPlan, OlcbpPlan* rightPlan) const
{
    size_t leftSize = leftPlan->Size();
    size_t rightSize = rightPlan->Size();
    if ((leftSize != rightSize) || leftSize == 1)
    {
        // If the plans sizes does not match or,
        // if the plan size is 1 step then they are not identical
        return false;
    }

    OlcbpPlan::NodeSet leftNodes = leftPlan->GetNodes();
    OlcbpPlan::NodeSet rightNodes = rightPlan->GetNodes();
    OlcbpPlan::NodeSet diff;

    set_difference(leftNodes.begin(), leftNodes.end(), rightNodes.begin(), rightNodes.end(), inserter(diff, diff.end()));

    return diff.empty();
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstration::IdenticalPlanDetection(vector<CookedPlan*> &p_cookedPlans) const
{
    for (unsigned i = 0; i < p_cookedPlans.size(); ++i)
    {
        for (unsigned j = i + 1; j < p_cookedPlans.size(); ++j)
        {
            LogInfo("Checking if plan %d is identical to %d", i, j);
            if (IsIdenticalPlan(p_cookedPlans[i]->pPlan, p_cookedPlans[j]->pPlan))
            {
                p_cookedPlans[j]->pPlan->Clear();
                p_cookedPlans[j]->pPlan->AddNode(p_cookedPlans[i]->Goal, p_cookedPlans[i]->Goal->Id());
            }
        }
    }
}
//--------------------------------------------------------------------------------------------------------------
bool IStrategizer::LearningFromHumanDemonstration::SubplansDetection(vector<CookedPlan*>& p_cookedPlans) const
{
    SubplansMap subplans;

    // Detecting the subplans and the matched nodes
    for (unsigned i = 0; i < p_cookedPlans.size(); ++i)
    {
        for (int j = i - 1; j >= 0; --j)
        {
            LogInfo("Checking if %d is supergraph of %d", i, j);
            OlcbpPlan::NodeSet matchedIds;

            if (IsSuperGraph(p_cookedPlans[i]->pPlan, p_cookedPlans[j]->pPlan, matchedIds))
            {
                subplans[i].push_back(SubsetPlanData(j, matchedIds));
            }
        }
    }

    if (subplans.empty())
    {
        return false;
    }

    for (unsigned i = 0; i < p_cookedPlans.size(); ++i)
    {
        // Remove duplicated nodes between subplans
        for (unsigned j = 0; j < subplans[i].size(); ++j)
        {
            for (unsigned k = j + 1; k < subplans[i].size(); ++k)
            {
                for (OlcbpPlan::NodeID nodeId : subplans[i][j].Nodes)
                {
                    subplans[i][k].Nodes.erase(nodeId);
                }
            }
        }

        for (SubsetPlanData subplanData : subplans[i])
        {
            if (subplanData.Nodes.empty())
            {
                continue;
            }

            unsigned subplanInx = subplanData.Index;
            unsigned subplanId = p_cookedPlans[subplanInx]->Goal->Id();
            unsigned nodeOrder = subplanId;
            OlcbpPlan::NodeSet matchedIds = subplanData.Nodes;
            GoalEx* pGoalClone = (GoalEx*)p_cookedPlans[subplanInx]->Goal->Clone();
            pGoalClone->Id(subplanId);
            
            if (p_cookedPlans[i]->pPlan->Contains(subplanId))
            {
                subplanId = PlanStepEx::GenerateID();
                pGoalClone->Id(subplanId);
            }
            
            // Substitute the detected subplans in the super plan
            _ASSERTE(subplanId == pGoalClone->Id());
            p_cookedPlans[i]->pPlan->SubGraphSubstitution(matchedIds, pGoalClone, subplanId, nodeOrder);

            LogInfo("Subistituting %d/%d", i, subplanInx);
        }
    }

    return true;
}
//--------------------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstration::IsSubset(const OlcbpPlan::NodeSet superset,  const OlcbpPlan::NodeSet candidateSubset) const
{
    if (superset.size() == 1 || candidateSubset.size() == 1)
    {
        return false;
    }

    OlcbpPlan::NodeSet intersect;
    set_intersection(superset.begin(), superset.end(), candidateSubset.begin(), candidateSubset.end(), std::inserter(intersect, intersect.end()));

    return intersect.size() == candidateSubset.size();
}
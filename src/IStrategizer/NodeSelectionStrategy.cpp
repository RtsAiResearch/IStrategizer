#include "NodeSelectionStrategy.h"
#include "OnlinePlanExpansionExecution.h"
#include "Logger.h"
#include <map>
#include <stack>

using namespace IStrategizer;
using namespace std;

void LfhdCbNodeSelector::Select(_Out_ IOlcbpPlan::NodeSet& goalsToUpdate,
    _Out_ IOlcbpPlan::NodeSet& actionsToUpdate,
    _Out_ IOlcbpPlan::NodeSet& snippetsToDestroy)
{
    IOlcbpPlan* pPlan = m_pPlanner->Plan();
    IOlcbpPlan::NodeQueue actionQ;
    IOlcbpPlan::NodeQueue goalQ;
    typedef unsigned GoalKey;
    map<GoalKey, stack<IOlcbpPlan::NodeID>> goalTable;

    // 1st pass: get ready nodes only
    m_pPlanner->GetReachableReadyNodes(actionQ, goalQ);

    while (!goalQ.empty())
    {
        IOlcbpPlan::NodeID currentGoalId = goalQ.front();
        goalQ.pop();

        if (!pPlan->Contains(currentGoalId))
            continue;

        GoalEx* pCurrGoal = (GoalEx*)pPlan->GetNode(currentGoalId);
        GoalKey typeKey = pCurrGoal->Key();

        if (!m_pPlanner->IsNodeDone(currentGoalId))
        {
            IOlcbpPlan::NodeID newActiveGoalId = currentGoalId;

            if (goalTable.count(typeKey) == 0)
                goalTable[typeKey].push(newActiveGoalId);
            else
            {
                auto& currentOverrideStack = goalTable[typeKey];
                IOlcbpPlan::NodeID currentActiveGoalId = currentOverrideStack.top();

                IOlcbpPlan::NodeSet ancestors;
                stack<IOlcbpPlan::NodeID> newOverrideStack;

                m_pPlanner->GetAncestorSatisfyingGoals(newActiveGoalId, ancestors);
                newOverrideStack.push(newActiveGoalId);

                while (!currentOverrideStack.empty())
                {
                    currentActiveGoalId = currentOverrideStack.top();

                    // Nodes belonging to my ancestors set should not be destroyed
                    // and should be kept in order
                    if (ancestors.count(currentActiveGoalId) != 0)
                    {
                        newOverrideStack.push(currentActiveGoalId);
                    }
                    else if (pPlan->Contains(currentActiveGoalId) &&
                        !m_pPlanner->IsNodeOpen(currentActiveGoalId) &&
                        !m_pPlanner->HasActiveAction(currentActiveGoalId))
                    {
                        snippetsToDestroy.insert(currentActiveGoalId);
                    }

                    currentOverrideStack.pop();
                }

                // Push back new stack into the current one in a LIFO order to reverse the original one
                while (!newOverrideStack.empty())
                {
                    currentOverrideStack.push(newOverrideStack.top());
                    newOverrideStack.pop();
                }
            }
        }
    }

    for (auto& r : goalTable)
    {
        goalsToUpdate.insert(r.second.top());
    }

    while (!actionQ.empty())
    {
        auto actionNodeId = actionQ.front();

        bool satisfyingGoalActive = goalsToUpdate.count(m_pPlanner->GetNodeData(actionNodeId).SatisfyingGoal) > 0;
        bool satisfyingGoalDone = m_pPlanner->IsNodeDone(m_pPlanner->GetNodeData(actionNodeId).SatisfyingGoal);

        if (satisfyingGoalActive || satisfyingGoalDone)
        {
            actionsToUpdate.insert(actionNodeId);
        }

        actionQ.pop();
    }
}
//////////////////////////////////////////////////////////////////////////
void GenCbNodeSelector::Select(_Out_ IOlcbpPlan::NodeSet& goalsToUpdate,
    _Out_ IOlcbpPlan::NodeSet& actionsToUpdate,
    _Out_ IOlcbpPlan::NodeSet& snippetsToDestroy)
{
    IOlcbpPlan* pPlan = m_pPlanner->Plan();
    IOlcbpPlan::NodeQueue actionQ;
    IOlcbpPlan::NodeQueue goalQ;

    // 1st pass: get ready nodes only
    m_pPlanner->GetReachableReadyNodes(actionQ, goalQ);

    while (!goalQ.empty())
    {
        auto goalNodeId = goalQ.front();
        if (!m_pPlanner->IsNodeDone(goalNodeId))
        {
            goalsToUpdate.insert(goalQ.front());

        }

        // Prune the plan by destroying snippets of succeeding goals to 
        // reduce plan noise and make it easier to read/visualize the plan
        if (pPlan->GetNode(goalNodeId)->GetState() == ESTATE_Succeeded &&
            !m_pPlanner->HasActiveAction(goalNodeId) &&
            m_pPlanner->IsGoalExpanded(goalNodeId))
        {
            snippetsToDestroy.insert(goalNodeId);
        }

        goalQ.pop();
    }

    while (!actionQ.empty())
    {
        auto actionNodeId = actionQ.front();

        if (!m_pPlanner->IsNodeDone(actionNodeId))
        {
            actionsToUpdate.insert(actionQ.front());
        }
        actionQ.pop();
    }
}
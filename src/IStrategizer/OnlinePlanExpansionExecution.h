#ifndef ONLINEPLANEXPANSIONEXECUTION_H
#define ONLINEPLANEXPANSIONEXECUTION_H

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif
#include "WorldClock.h"
#include "GoalEx.h"
#include "OlcbpPlanNodeData.h"
#include "CaseEx.h"

namespace IStrategizer
{
    enum PlayerType;
    class CaseEx;
    class GoalEx;
    class CaseBasedReasonerEx;

    class OnlinePlanExpansionExecution : public EngineComponent
    {
    public:
        typedef unsigned GoalTypeID;
        typedef IOlcbpPlan::NodeValue CaseNodeValue;
        typedef IOlcbpPlan::NodeValue ClonedCaseNodeValue;

        OnlinePlanExpansionExecution(_In_ GoalEx* pInitialGoal, _In_ CaseBasedReasonerEx* pCbReasoner);
        OnlinePlanExpansionExecution(_In_ GoalType goalType, _In_ CaseBasedReasonerEx *pCasedBasedReasoner);

        void Update(_In_ const WorldClock& clock);
        void NotifyMessegeSent(_In_ Message* pMessage);
        const IOlcbpPlan* Plan() const { return m_pOlcbpPlan; }
        IOlcbpPlan* Plan() { return m_pOlcbpPlan; }
        ConstOlcbpPlanNodeDataMapRef NodeData() const { return m_nodeData; }

    private:
        void ExpandGoal(IOlcbpPlan::NodeID goalNode, CaseEx* pCase);

        void UpdateBelongingSubplanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId);
        void UpdateGoalPlanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId);
        bool DestroyGoalPlanIfExist(_In_ IOlcbpPlan::NodeID planGoalNodeId);
        void AddReadyChildrenToUpdateQueue(_In_ IOlcbpPlan::NodeID nodeId, _Inout_ IOlcbpPlan::NodeQueue &updateQ);
        void UpdateActionNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock, _Inout_ IOlcbpPlan::NodeQueue& updateQ);
        void UpdateGoalNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock);

        void MarkCaseAsTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase);
        CaseEx* GetLastCaseForGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).BelongingCase; }
        bool IsCaseTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase) const { return GetNodeData(nodeId).TriedCases.count(pCase) > 0; }

        void OpenNode(_In_ IOlcbpPlan::NodeID nodeId) { LogInfo("Opening node %s", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str()); GetNodeData(nodeId).IsOpen = true; }
        void CloseNode(_In_ IOlcbpPlan::NodeID nodeId) { LogInfo("Closing node %s", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str()); GetNodeData(nodeId).IsOpen = false; }
        bool IsNodeOpen(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).IsOpen == true; }
        bool IsNodeReady(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).WaitOnParentsCount == 0; }
        bool IsNodeDone(_In_ IOlcbpPlan::NodeID nodeId) const 
        {
            auto state = m_pOlcbpPlan->GetNode(nodeId)->State();
            return state == ESTATE_Succeeded || state == ESTATE_Failed;
        }

        void SetGoalNodeBelongingCase(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase) { GetNodeData(nodeId).BelongingCase = pCase; }
        void SetNodeSatisfyingGoal(_In_ IOlcbpPlan::NodeID nodeId, _In_ IOlcbpPlan::NodeID goalNodeId) { GetNodeData(nodeId).SatisfyingGoal = goalNodeId; }
        void OnNodeDone(_In_ IOlcbpPlan::NodeID nodeId);

        bool IsGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const;
        bool IsActionNode(_In_ IOlcbpPlan::NodeID nodeId) const;
        bool IsGoalTypeAssigned(_In_ GoalTypeID typeId) const { return m_goalTypeAssignment.count(typeId) > 0 && m_goalTypeAssignment.at(typeId) != IOlcbpPlan::NullNodeID; }

        void AssignGoalType(_In_ GoalTypeID typeId, _In_ IOlcbpPlan::NodeID assignedNodeId) 
        {
            _ASSERTE(!IsGoalTypeAssigned(typeId));
            LogInfo("Marking goal type with Id=%d as assigned to node %s", typeId, m_pOlcbpPlan->GetNode(assignedNodeId)->ToString().c_str());
            m_goalTypeAssignment[typeId] = assignedNodeId;
        }

        void UnassignGoalType(_In_ GoalTypeID typeId) 
        {
            _ASSERTE(IsGoalTypeAssigned(typeId));
            LogInfo("Marking goal type with Id=%d as non-assigned, was assigned by node %s", typeId, m_pOlcbpPlan->GetNode(m_goalTypeAssignment[typeId])->ToString().c_str());
            m_goalTypeAssignment[typeId] = IOlcbpPlan::NullNodeID; 
        }

        bool TryToAcquireGoalType(_In_ GoalTypeID typeId, _In_ IOlcbpPlan::NodeID nodeIdToAssign)
        {
            if (!IsGoalTypeAssigned(typeId))
            {
                AssignGoalType(typeId, nodeIdToAssign);
                return true;
            }
            else if (m_goalTypeAssignment[typeId] == nodeIdToAssign)
                return true;
            else
                return false;
        }

        void LinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId);
        void UnlinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId);

        OlcbpPlanNodeData& GetNodeData(_In_ IOlcbpPlan::NodeID nodeId) { _ASSERTE(m_nodeData.count(nodeId) > 0); return m_nodeData[nodeId]; }
        const OlcbpPlanNodeData& GetNodeData(_In_ IOlcbpPlan::NodeID nodeId) const { _ASSERTE(m_nodeData.count(nodeId) > 0); return m_nodeData.at(nodeId); }

        void GetNodeChildrenInBelongingSubplan(_In_ IOlcbpPlan::NodeID nodeId, _Out_ IOlcbpPlan::NodeSerializedSet& children) const;
        void UnlinkNodeChildren(_In_ IOlcbpPlan::NodeID nodeId);
        void ComputeNodesWaitOnParentsCount();
        void UpdateHistory(CaseEx* pCase);

        void OnGoalNodeSucceeded(_In_ IOlcbpPlan::NodeID nodeId);
        void OnGoalNodeFailed(_In_ IOlcbpPlan::NodeID nodeId);

        CaseBasedReasonerEx *m_pCbReasoner;
        OlcbpPlanNodeDataMap m_nodeData;
        IOlcbpPlan::NodeID m_planRootNodeId;
        IOlcbpPlan *m_pOlcbpPlan;
        bool m_planStructureChangedThisFrame;
        std::map<CaseNodeValue, ClonedCaseNodeValue> m_clonedNodesMapping;
        std::map<GoalTypeID, IOlcbpPlan::NodeID> m_goalTypeAssignment;
        CaseSet m_rootNodeExclusions;
        GoalType m_rootGoalType;
    };
}

#endif // ONLINEPLANEXPANSIONEXECUTION_H
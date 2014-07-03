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

        void Update(_In_ const WorldClock& clock);
        void NotifyMessegeSent(_In_ Message* pMessage);
        void StartPlanning();
        void RootGoal(GoalEx* pGoal) { _ASSERTE(pGoal); m_pRootGoal = pGoal; }
        const IOlcbpPlan* Plan() const { return m_pOlcbpPlan; }
        IOlcbpPlan* Plan() { return m_pOlcbpPlan; }
        ConstOlcbpPlanNodeDataMapRef NodeData() const { return m_nodeData; }

    private:
        bool IsGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const { return BELONG(GoalType, m_pOlcbpPlan->GetNode(nodeId)->StepTypeId()); }
        bool IsActionNode(_In_ IOlcbpPlan::NodeID nodeId) const { return BELONG(ActionType, m_pOlcbpPlan->GetNode(nodeId)->StepTypeId()); }
        bool IsGoalTypeAssigned(_In_ GoalTypeID typeId) const { return m_goalTypeAssignment.count(typeId) > 0 && m_goalTypeAssignment.at(typeId) != IOlcbpPlan::NullNodeID; }
        bool IsNodeOpen(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).IsOpen == true; }
        bool IsNodeReady(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).WaitOnParentsCount == 0; }
        bool IsNodeDone(_In_ IOlcbpPlan::NodeID nodeId) const { auto state = m_pOlcbpPlan->GetNode(nodeId)->State(); return state == ESTATE_Succeeded || state == ESTATE_Failed; }
        bool IsCaseTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase) const { return GetNodeData(nodeId).TriedCases.count(pCase) > 0; }

        OlcbpPlanNodeData& GetNodeData(_In_ IOlcbpPlan::NodeID nodeId) { _ASSERTE(m_nodeData.count(nodeId) > 0); return m_nodeData[nodeId]; }
        const OlcbpPlanNodeData& GetNodeData(_In_ IOlcbpPlan::NodeID nodeId) const { _ASSERTE(m_nodeData.count(nodeId) > 0); return m_nodeData.at(nodeId); }
        CaseEx* GetLastCaseForGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).BelongingCase; }
        void GetSnippetOrphanNodes(_In_ IOlcbpPlan::NodeID snippetRootId, _Out_ IOlcbpPlan::NodeSerializedSet& orphans);
        void SetGoalNodeBelongingCase(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase) { GetNodeData(nodeId).BelongingCase = pCase; }
        void SetNodeSatisfyingGoal(_In_ IOlcbpPlan::NodeID nodeId, _In_ IOlcbpPlan::NodeID goalNodeId) { GetNodeData(nodeId).SatisfyingGoal = goalNodeId; }

        void ExpandGoal(IOlcbpPlan::NodeID goalNode, CaseEx* pCase);
        void UpdateBelongingSubplanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId);
        bool DestroyGoalSnippetIfExist(_In_ IOlcbpPlan::NodeID planGoalNodeId);
        void AddReadyChildrenToUpdateQueue(_In_ IOlcbpPlan::NodeID nodeId, _Inout_ IOlcbpPlan::NodeQueue &updateQ);
        void UpdateActionNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock);
        void UpdateGoalNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock);
        void LinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId) { m_pOlcbpPlan->AddEdge(srcNodeId, dstNodeId); }
        void ComputeFreshSnippetWaitOnParentsCount(_In_ IOlcbpPlan::NodeID subGraphRootId);
        void UpdateHistory(CaseEx* pCase);
        void MarkCaseAsTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase);
        void OpenNode(_In_ IOlcbpPlan::NodeID nodeId) { LogInfo("Opening node %s", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str()); GetNodeData(nodeId).IsOpen = true; }
        void CloseNode(_In_ IOlcbpPlan::NodeID nodeId) { LogInfo("Closing node %s", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str()); GetNodeData(nodeId).IsOpen = false; }
        void GetReachableReadyNodes(_Out_ IOlcbpPlan::NodeQueue& actionQ, _Out_ IOlcbpPlan::NodeQueue& goalQ);

        void OnGoalNodeSucceeded(_In_ IOlcbpPlan::NodeID nodeId);
        void OnGoalNodeFailed(_In_ IOlcbpPlan::NodeID nodeId);
        void OnActionNodeSucceeded(_In_ IOlcbpPlan::NodeID nodeId);
        void OnActionNodeFailed(_In_ IOlcbpPlan::NodeID nodeId);
        void OnNodeDone(_In_ IOlcbpPlan::NodeID nodeId);

        CaseBasedReasonerEx *m_pCbReasoner;
        OlcbpPlanNodeDataMap m_nodeData;
        IOlcbpPlan::NodeID m_planRootNodeId;
        IOlcbpPlan *m_pOlcbpPlan;
        bool m_planStructureChangedThisFrame;
        std::map<CaseNodeValue, ClonedCaseNodeValue> m_clonedNodesMapping;
        std::map<GoalTypeID, IOlcbpPlan::NodeID> m_goalTypeAssignment;
        GoalType m_rootGoalType;
        GoalEx* m_pRootGoal;
    };
}

#endif // ONLINEPLANEXPANSIONEXECUTION_H
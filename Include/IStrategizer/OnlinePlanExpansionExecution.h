#ifndef ONLINEPLANEXPANSIONEXECUTION_H
#define ONLINEPLANEXPANSIONEXECUTION_H

#include <map>
#include "EngineData.h"
#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif
#include "WorldClock.h"

namespace IStrategizer
{
    enum PlayerType;
    class CaseEx;
    class GoalEx;
    class CaseBasedReasonerEx;

    class OnlinePlanExpansionExecution : public EngineComponent
    {
    public:
        typedef std::set<CaseEx*> CaseSet;

        OnlinePlanExpansionExecution(GoalEx* pInitialGoal, CaseBasedReasonerEx* pCbReasoner);
        void Update(const WorldClock& clock);
        void NotifyMessegeSent(Message* pMessage);
        const IOlcbpPlan* Plan() const { return m_pOlcbpPlan; }
        IOlcbpPlan* Plan() { return m_pOlcbpPlan; }

    private:
        class NodeData
        {
        public:
            CaseSet TriedCases;
            CaseEx* BelongingCase;
            IOlcbpPlan::NodeID SatisfyingGoal;
            unsigned NotReadyParentsCount;
            bool IsOpen;

            NodeData()
                : BelongingCase(nullptr),
                SatisfyingGoal(IOlcbpPlan::NullNodeID),
                NotReadyParentsCount(0),
                IsOpen(true)
            { }
        };

        void ExpandGoal(IOlcbpPlan::NodeID goalNode, CaseEx* pCase);

        void UpdateNodeChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId);
        bool DestroyGoalPlanIfExist(_In_ IOlcbpPlan::NodeID planGoalNodeId);
        void AddReadyChildrenToUpdateQueue(_In_ IOlcbpPlan::NodeID nodeId, _Inout_ IOlcbpPlan::NodeQueue &updateQ);
        void UpdateActionNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock, _Inout_ IOlcbpPlan::NodeQueue& updateQ);
        void UpdateGoalNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock, _Inout_ IOlcbpPlan::NodeQueue& updateQ);

        void MarkCaseAsTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase);
        CaseEx* GetLastCaseForGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).BelongingCase; }
        bool IsCaseTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase) const { return GetNodeData(nodeId).TriedCases.count(pCase) > 0; }

        void OpenNode(_In_ IOlcbpPlan::NodeID nodeId) { GetNodeData(nodeId).IsOpen = true; }
        void CloseNode(_In_ IOlcbpPlan::NodeID nodeId) { GetNodeData(nodeId).IsOpen = false; }
        bool IsNodeOpen(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).IsOpen == true; }
        bool IsNodeReady(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).NotReadyParentsCount == 0; }

        void SetGoalNodeBelongingCase(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase) { GetNodeData(nodeId).BelongingCase = pCase; }
        void SetNodeSatisfyingGoal(_In_ IOlcbpPlan::NodeID nodeId, _In_ IOlcbpPlan::NodeID goalNodeId) { GetNodeData(nodeId).SatisfyingGoal = goalNodeId; }

        bool IsGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const;
        bool IsActionNode(_In_ IOlcbpPlan::NodeID nodeId) const;
        void LinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId);
        void UnlinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId);

        NodeData& GetNodeData(_In_ IOlcbpPlan::NodeID nodeId) { _ASSERTE(m_nodeData.count(nodeId) > 0); return m_nodeData[nodeId]; }
        const NodeData& GetNodeData(_In_ IOlcbpPlan::NodeID nodeId) const { _ASSERTE(m_nodeData.count(nodeId) > 0); return m_nodeData.at(nodeId); }

        void GetNodeChildrenInBelongingSubplan(_In_ IOlcbpPlan::NodeID nodeId, _Out_ IOlcbpPlan::NodeSet& children) const;
        void UnlinkNodeChildren(_In_ IOlcbpPlan::NodeID nodeId);
        void ComputeNodesNotReadyParents();

        CaseBasedReasonerEx *m_pCbReasoner;
        std::map<IOlcbpPlan::NodeID, NodeData> m_nodeData;
        IOlcbpPlan::NodeID m_planRootNodeId;
        IOlcbpPlan *m_pOlcbpPlan;
        bool m_planStructureChangedThisFrame;
    };
}

#endif // ONLINEPLANEXPANSIONEXECUTION_H
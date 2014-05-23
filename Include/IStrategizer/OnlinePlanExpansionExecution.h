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
            IOlcbpPlan::NodeID ID;
            CaseSet TriedCases;
            CaseEx* BelongingCase;
            IOlcbpPlan::NodeID SatisfyingGoal;
            unsigned WaitOnParentsCount;
            unsigned WaitOnChildrenCount;
            bool IsOpen;
            bool IsDone;

            NodeData()
                :
                ID(IOlcbpPlan::NullNodeID),
                BelongingCase(nullptr),
                SatisfyingGoal(IOlcbpPlan::NullNodeID),
                WaitOnParentsCount(0),
                WaitOnChildrenCount(0),
                IsDone(false),
                IsOpen(false)
            { }

            void DecWaitOnParentsCount() { LogInfo("Dec WaitOnParentsCount=%d for node[%x]", WaitOnParentsCount, ID); --WaitOnParentsCount; }
            void IncWaitOnParentsCount() { LogInfo("Inc WaitOnParentsCount=%d for node[%x]", WaitOnParentsCount, ID); ++WaitOnParentsCount; }
            void SetWaitOnParentsCount(unsigned val) { LogInfo("Set WaitOnParentsCount= %d -> %d for node[%x]", WaitOnParentsCount, val, ID); WaitOnParentsCount = val; }
            void DecWaitOnChildrenCount() { LogInfo("Dec WaitOnChildrenCount=%d for node[%x]", WaitOnChildrenCount, ID); --WaitOnChildrenCount; }
            void SetWaitOnChildrenCount(unsigned val) { LogInfo("Set WaitOnChildrenCount= %d -> %d for node[%x]", WaitOnChildrenCount, val, ID); WaitOnChildrenCount = val; }
        };

        void ExpandGoal(IOlcbpPlan::NodeID goalNode, CaseEx* pCase);

        void UpdateBelongingSubplanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId);
        void UpdateGoalPlanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId);
        bool DestroyGoalPlanIfExist(_In_ IOlcbpPlan::NodeID planGoalNodeId);
        void AddReadyChildrenToUpdateQueue(_In_ IOlcbpPlan::NodeID nodeId, _Inout_ IOlcbpPlan::NodeQueue &updateQ);
        void UpdateActionNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock, _Inout_ IOlcbpPlan::NodeQueue& updateQ);
        void UpdateGoalNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock, _Inout_ IOlcbpPlan::NodeQueue& updateQ);

        void MarkCaseAsTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase);
        CaseEx* GetLastCaseForGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).BelongingCase; }
        bool IsCaseTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase) const { return GetNodeData(nodeId).TriedCases.count(pCase) > 0; }

        void OpenNode(_In_ IOlcbpPlan::NodeID nodeId) { GetNodeData(nodeId).IsOpen = true; }
        void CloseNode(_In_ IOlcbpPlan::NodeID nodeId) { GetNodeData(nodeId).IsOpen = false; }
        void MarkNodeAsDone(_In_ IOlcbpPlan::NodeID nodeId) { GetNodeData(nodeId).IsDone = true; OnNodeDone(nodeId); }
        bool IsNodeOpen(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).IsOpen == true; }
        bool IsNodeReady(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).WaitOnParentsCount == 0; }
        bool IsNodeDone(_In_ IOlcbpPlan::NodeID nodeId) const { return GetNodeData(nodeId).IsDone == true; }

        void SetGoalNodeBelongingCase(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase) { GetNodeData(nodeId).BelongingCase = pCase; }
        void SetNodeSatisfyingGoal(_In_ IOlcbpPlan::NodeID nodeId, _In_ IOlcbpPlan::NodeID goalNodeId) { GetNodeData(nodeId).SatisfyingGoal = goalNodeId; }
        void OnNodeDone(_In_ IOlcbpPlan::NodeID nodeId);

        bool IsGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const;
        bool IsActionNode(_In_ IOlcbpPlan::NodeID nodeId) const;
        void LinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId);
        void UnlinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId);

        NodeData& GetNodeData(_In_ IOlcbpPlan::NodeID nodeId) { _ASSERTE(m_nodeData.count(nodeId) > 0); return m_nodeData[nodeId]; }
        const NodeData& GetNodeData(_In_ IOlcbpPlan::NodeID nodeId) const { _ASSERTE(m_nodeData.count(nodeId) > 0); return m_nodeData.at(nodeId); }

        void GetNodeChildrenInBelongingSubplan(_In_ IOlcbpPlan::NodeID nodeId, _Out_ IOlcbpPlan::NodeSet& children) const;
        void UnlinkNodeChildren(_In_ IOlcbpPlan::NodeID nodeId);
        void ComputeNodesWaitOnParentsCount();
        void UpdateHistory(CaseEx* pCase);
        CaseBasedReasonerEx *m_pCbReasoner;
        std::map<IOlcbpPlan::NodeID, NodeData> m_nodeData;
        IOlcbpPlan::NodeID m_planRootNodeId;
        IOlcbpPlan *m_pOlcbpPlan;
        bool m_planStructureChangedThisFrame;
        
        typedef IOlcbpPlan::NodeValue CaseNodeValue;
        typedef IOlcbpPlan::NodeValue ClonedCaseNodeValue;
        std::map<CaseNodeValue, ClonedCaseNodeValue> m_clonedNodesMapping;
    };
}

#endif // ONLINEPLANEXPANSIONEXECUTION_H
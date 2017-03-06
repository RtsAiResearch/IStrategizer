#ifndef DENSECBNODESELECTOR_H
#define DENSECBNODESELECTOR_H

#include "EngineData.h"

namespace IStrategizer
{
    class OnlinePlanExpansionExecution;

    class NodeSelectionStrategy
    {
    public:
        NodeSelectionStrategy(OnlinePlanExpansionExecution* pPlanner) :
            m_pPlanner(pPlanner)
        {}

        virtual void Select(_Out_ IOlcbpPlan::NodeSet& goalsToUpdate,
            _Out_ IOlcbpPlan::NodeDQueue& actionsToUpdate,
            _Out_ IOlcbpPlan::NodeSet& snippetsToDestroy) = 0;

    protected:
        OnlinePlanExpansionExecution* m_pPlanner;
    };

    class LfhdCbNodeSelector : public NodeSelectionStrategy
    {
    public:
        LfhdCbNodeSelector(OnlinePlanExpansionExecution* pPlanner) :
            NodeSelectionStrategy(pPlanner) {}

        void Select(_Out_ IOlcbpPlan::NodeSet& goalsToUpdate,
            _Out_ IOlcbpPlan::NodeDQueue& actionsToUpdate,
            _Out_ IOlcbpPlan::NodeSet& snippetsToDestroy);
    };

    class GenCbNodeSelector : public NodeSelectionStrategy
    {
    public:
        GenCbNodeSelector(OnlinePlanExpansionExecution* pPlanner) :
            NodeSelectionStrategy(pPlanner) {}

        void Select(_Out_ IOlcbpPlan::NodeSet& goalsToUpdate,
            _Out_ IOlcbpPlan::NodeDQueue& actionsToUpdate,
            _Out_ IOlcbpPlan::NodeSet& snippetsToDestroy);
    };
}

#endif // DENSECBNODESELECTOR_H
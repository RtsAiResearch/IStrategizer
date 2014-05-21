///> [Serializable]
#ifndef ADJLISTDIGRAPH_H
#define ADJLISTDIGRAPH_H

#include <mutex>
#include <stack>
#include "IDigraph.h"
#include "SMap.h"
#include "SPair.h"
#include "SSet.h"
#include "UserObject.h"
#include "Logger.h"

namespace IStrategizer
{
    template<class T>
    struct AdjListDigraphNodeValueTraits
    {
        typedef T Type;
        typedef const T ConstType;
        static std::string ToString(ConstType& v) { return std::to_string(v); }
    };

    ///> class=AdjListDigraph(TNodeValue)
    ///> parent=IDigraph(TNodeValue)
    template<class TNodeValue, class TNodeValueTraits = AdjListDigraphNodeValueTraits<TNodeValue>>
    class AdjListDigraph :  public Serialization::UserObject, public IDigraph<TNodeValue>
    {
    public:
        ///> alias=NodeEntry(pair(NodeValue,NodeSet))
        typedef Serialization::SPair<NodeValue, NodeSet> NodeEntry;

        AdjListDigraph()
            : m_lastNodeId(0)
        {
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::AddNode
        // Description:	Add a new node to the Digraph without connecting it and identify it using
        // the passed in id instead of auto generating a new id
        // Parameter: 	NodeValue val: A data value that is associated with the added node
        // Parameter:   NodeID:  A unique ID used to reference the added node
        // in further Digraph methods
        //************************************
        NodeID AddNode(const _In_ NodeValue& val, NodeID id)
            throw(ItemAlreadyExistsException)
        {
            if (m_adjList.count(id) > 0)
                DEBUG_THROW(ItemAlreadyExistsException(XcptHere));

            m_adjList.insert(make_pair(id, MakePair(val, NodeSet())));
            m_lastNodeId = id;

            return id;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::RemoveNode
        // Description:	Disconnect a node from the digraph and removes it
        // Parameter: 	NodeID id: Unique ID to identify the node
        // Returns:   	void
        //************************************
        void RemoveNode(_In_ NodeID id)
            throw(ItemNotFoundException)
        {
            if (m_adjList.count(id) == 0)
                DEBUG_THROW(ItemNotFoundException(XcptHere));

            // Disconnect the node from graph nodes
            for (Serialization::SMap<NodeID, NodeEntry>::iterator nodeEntryItr = m_adjList.begin();
                nodeEntryItr != m_adjList.end(); ++nodeEntryItr)
            {
                NodeSet& adjNodes = nodeEntryItr->second.second;
                if (adjNodes.count(id) > 0)
                {
                    adjNodes.erase(id);
                }
            }

            m_adjList.erase(id);
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::AddEdge
        // Description:	Create a directed edge that goes from sourceNode to a destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	void
        //************************************
        void AddEdge(_In_ NodeID sourceNodeId, _In_ NodeID destNodeId) 
            throw(ItemAlreadyExistsException)
        {
            if (m_adjList.count(sourceNodeId) == 0 ||
                m_adjList.count(destNodeId) == 0)
                DEBUG_THROW(ItemNotFoundException(XcptHere));

            m_adjList[sourceNodeId].second.insert(destNodeId);
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::RemoveEdge
        // Description:	Removes the edge that goes from sourceNode to destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	void
        //************************************
        void RemoveEdge(_In_ NodeID sourceNodeId, _In_ NodeID destNodeId) 
            throw(ItemNotFoundException)
        {
            if (m_adjList.count(sourceNodeId) == 0 ||
                m_adjList.count(destNodeId) == 0)
                DEBUG_THROW(ItemNotFoundException(XcptHere));

            m_adjList[sourceNodeId].second.erase(destNodeId);
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetNode
        // Description:	Get the value associated with the node
        // Parameter: 	NodeID id: Unique ID to identify the node
        // Returns:   	IStrategizer::NodeValue
        //************************************
        NodeValue& GetNode(_In_ NodeID id)
            throw(ItemNotFoundException)
        {
            if (m_adjList.count(id) == 0)
                DEBUG_THROW(ItemNotFoundException(XcptHere));

            return m_adjList[id].first;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetNodes
        // Description:	Returns a set of all node ids inside the digraph
        // Returns:   	NodeSet
        //************************************
        NodeSet GetNodes() const
        {
            NodeSet nodes;

            for (auto nodeEntry : m_adjList)
            {
                nodes.insert(nodeEntry.first);
            }

            return nodes;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Size
        // Description:	Returns the number of nodes inside the digraph
        // Returns:   	size_t
        //************************************
        size_t Size() const { return m_adjList.size(); }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Clear
        // Description:	Delete all graph nodes and edges
        // Returns:   	void
        //************************************
        virtual void Clear() { m_adjList.clear(); }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::IsAdjacent
        // Description:	Check whether there is an edge that goes from sourceNode to destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	bool
        //************************************
        bool IsAdjacent(_In_ NodeID sourceNodeId, _In_ NodeID destNodeId) const 
        {
            if (m_adjList.count(sourceNodeId) == 0 ||
                m_adjList.count(destNodeId) == 0)
                return false;

            return m_adjList.at(sourceNodeId).second.count(destNodeId) > 0;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetAdjacentNodes
        // Description:	Get all nodes that has an edge going from sourceNode to it
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Returns:   	NodeSet: A set of all node ids adjacent to sourceNode
        //************************************
        const NodeSet& GetAdjacentNodes(_In_ NodeID sourceNodeId) const 
            throw(ItemNotFoundException)
        {
            if (m_adjList.count(sourceNodeId) == 0)
                DEBUG_THROW(ItemNotFoundException(XcptHere));

            return m_adjList.at(sourceNodeId).second;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::SubGraphSubstitution
        // Description:	Replaces a sub-part of the IDigraph with the given TNodeValue provided.
        // Parameter: 	NodeSet subGraphIds: The ids describing the sub-part to replace.
        // Parameter:   TNodeValue substitute: The TNodeValue to replace the sub-part with.
        //************************************      
        void SubGraphSubstitution(_In_ NodeSet subGraphIds, _In_ NodeValue substitute, _In_ NodeID substituteId)
        {
            std::string subGraphIdSet = "{";

            LogInfo("Substituting %d with %s", substituteId, ToString(subGraphIds).c_str());
            LogInfo("Pre-process graph state:\n%s\n", ToString().c_str());

            AddNode(substitute, substituteId);

            NodeSet parents;
            NodeSet children;

            for (NodeID subGraphNodeId : subGraphIds)
            {
                NodeSet tempParents = GetParents(subGraphNodeId);

                parents.insert(tempParents.begin(), tempParents.end());

                NodeSet tempChildren = GetAdjacentNodes(subGraphNodeId);
                children.insert(tempChildren.begin(), tempChildren.end());
            }

            for (auto id : subGraphIds)
            {
                parents.erase(id);
                children.erase(id);
            }

            for (NodeID subGraphNodeId : subGraphIds) RemoveNode(subGraphNodeId);

            for (NodeID parent : parents) AddEdge(parent, substituteId);
            for (NodeID child : children) AddEdge(substituteId, child);

            LogInfo("Post-process graph state:\n%s\n", ToString().c_str());
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetOrphanNodes
        // Description:	Get all nodes that do not have an edge going from any node to them
        // Returns:   	NodeSet: A set of all orphan node ids
        //************************************
        NodeSet GetOrphanNodes() const
        {
            // Algorithm:
            // Assume all nodes are orphans
            // ..O is set of all graph nodes
            // ..For each node N in the graph G
            // ....For each adj node A in N adj nodes
            // ......Remove A from O
            NodeSet orphans = GetNodes();

            for (auto nodeEntry : m_adjList)
            {
                NodeSet& adjacents = nodeEntry.second.second;

                for (auto adjNodeId : adjacents)
                {
                    orphans.erase(adjNodeId);
                }
            }

            return orphans;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetLeafNodes
        // Description:	Get all nodes that do not have an edge going from them to any node
        // Returns:   	NodeSet
        //************************************
        NodeSet GetLeafNodes() const
        {
            NodeSet leaves;

            for (auto nodeEntry : m_adjList)
            {
                NodeSet& adjacents = nodeEntry.second.second;

                if (adjacents.empty())
                    leaves.insert(nodeEntry.first);
            }

            return leaves;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::PathExists
        // Description:	Checks if there's a path from start node to the end node.
        // Parameter: 	NodeID start: The start node ID
        // Parameter:   NodeID end: the end node id
        //// Returns:   bool: True if path exists, false otherwise.
        //************************************
        bool PathExists(const NodeID start, const NodeID end) const
        {
            stack<NodeID> nodes;

            for (NodeID childId : GetAdjacentNodes(start)) nodes.push(childId);

            while (!nodes.empty())
            {
                NodeID currentNode = nodes.top();
                if (currentNode == end)
                {
                    return true;
                }
                else
                {
                    nodes.pop();
                    for (NodeID childId : GetAdjacentNodes(currentNode)) nodes.push(childId);
                }
            }

            return false;
        }

        bool IsSuperGraphOf(AdjListDigraph<TNodeValue, TNodeValueTraits> *pCandidateSubGraph, NodeSet& superGraphMatchedIds)
        {
            if (Size() <= pCandidateSubGraph->Size())
            {
                return false;
            }

            NodeSet subGraphMatchedIds;
            return IsSuperGraph(pCandidateSubGraph->GetOrphanNodes(), GetNodes(), subGraphMatchedIds, superGraphMatchedIds, pCandidateSubGraph);
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Lock
        // Description:	Locks the graph for exclusive read/write access by the caller thread
        // Returns:   	void
        //************************************
        void Lock() { m_lock.lock(); }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Unlock
        // Description:	Unlocks the graph acquisition by caller thread
        // Returns:   	void
        //************************************
        void Unlock() { m_lock.unlock(); }        OBJECT_SERIALIZABLE(AdjListDigraph);

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::ToString
        // Description:	Visualize the digraph in a string representation that describes
        // its nodes and the connections between them
        // Returns:   	std::string
        //************************************
        std::string ToString() const
        {
            std::string str;
            char strID[32];

            NodeSet roots = GetOrphanNodes();
            str = "R = ";
            str += ToString(roots);
            str += "\n";

            for (auto nodeEntry : m_adjList)
            {
                if (m_adjList.count(nodeEntry.first) == 0)
                    DEBUG_THROW(ItemNotFoundException(XcptHere));

                AdjListDigraphNodeValueTraits<TNodeValue>::ConstType nodeVal = m_adjList.at(nodeEntry.first).first;
                str += TNodeValueTraits::ToString(nodeVal);
                str += '[';
                sprintf_s(strID, "%x", nodeEntry.first);
                str += strID;
                str += ']';
                str += " -> ";

                NodeSet& adjacents = nodeEntry.second.second;

                for (auto adjNodeID : adjacents)
                {
                    if (m_adjList.count(adjNodeID) == 0)
                        DEBUG_THROW(ItemNotFoundException(XcptHere));

                    AdjListDigraphNodeValueTraits<TNodeValue>::ConstType adjNodeVal = m_adjList.at(adjNodeID).first;

                    str += TNodeValueTraits::ToString(adjNodeVal);
                    str += '[';
                    sprintf_s(strID, "%x", adjNodeID);
                    str += strID;
                    str += ']';
                    str += ',';
                }

                str += '\n';
            }

            return str;
        }

        OBJECT_MEMBERS(2 ,&m_lastNodeId, &m_adjList);

    private:
        ///> type=NodeID
        NodeID m_lastNodeId;
        ///> type=map(pair(NodeID,NodeEntry))
        Serialization::SMap<NodeID, NodeEntry> m_adjList;

        std::mutex m_lock;

        template<class T>
        std::string ToString(const T& s) const
        {
            char strID[32];
            std::string strIdSet;

            strIdSet += "{";

            for (NodeID id : s)
            {
                sprintf_s(strID, "%x", id);
                strIdSet += strID;
                strIdSet += ",";
            }

            strIdSet += "}";

            return strIdSet;
        }

        NodeSet GetParents(NodeID nodeId) const
        {
            if (m_adjList.count(nodeId) == 0)
                throw ItemNotFoundException(XcptHere);

            NodeSet m_parents;

            for each(auto parent in m_adjList)
            {
                if(parent.second.second.count(nodeId) > 0)
                {
                    m_parents.insert(parent.first);
                }
            }

            return m_parents;
        }

        bool IsSuperGraph(
            const NodeSet& subGraphChildren,
            const NodeSet& superGraphChildren,
            NodeSet& subGraphMatchedIds,
            NodeSet& superGraphMatchedIds,
            AdjListDigraph<TNodeValue, TNodeValueTraits> *pCandidateSubGraph)
        {
            if (subGraphChildren.empty())
            {
                // Base case, that the there are no children to match
                // and everything previously matched, then return true.
                return true;
            }
            else if (superGraphChildren.empty())
            {
                // The subgraph still have nodes to match but
                // the supergraph does not have any.
                return false;
            }

            for (NodeID supGraphNodeId : subGraphChildren)
            {
                bool nodeMatched = false;
                
                if (subGraphMatchedIds.count(supGraphNodeId) != 0)
                {
                    // If we matched this subGraphNode before then no need to get into its children
                    // and try matching them again, just skip this node.
                    continue;
                }

                for (NodeID superGraphNodeId : superGraphChildren)
                {
                    IComparable* pLeft = dynamic_cast<IComparable*>(pCandidateSubGraph->GetNode(supGraphNodeId));
                    IComparable* pRight = dynamic_cast<IComparable*>(GetNode(superGraphNodeId));

                    if (pLeft->Compare(pRight) == 0 && superGraphMatchedIds.count(superGraphNodeId) == 0)
                    {
                        bool childrenMatched = IsSuperGraph(
                            pCandidateSubGraph->GetAdjacentNodes(supGraphNodeId),
                            GetAdjacentNodes(superGraphNodeId),
                            subGraphMatchedIds,
                            superGraphMatchedIds,
                            pCandidateSubGraph);

                        if (childrenMatched)
                        {
                            superGraphMatchedIds.insert(superGraphNodeId);
                            subGraphMatchedIds.insert(supGraphNodeId);
                            nodeMatched = true;
                            break;
                        }
                    }
                }

                if (!nodeMatched)
                {
                    return false;
                }
            }

            return true;
        }
    };
}

#endif
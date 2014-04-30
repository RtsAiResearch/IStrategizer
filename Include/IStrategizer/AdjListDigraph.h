///> [Serializable]
#ifndef ADJLISTDIGRAPH_H
#define ADJLISTDIGRAPH_H

#include <mutex>
#include "IDigraph.h"
#include "SMap.h"
#include "SPair.h"
#include "SSet.h"
#include "UserObject.h"

#include <vector>
#include <unordered_set>
using namespace std;

namespace IStrategizer
{
    ///> class=AdjListDigraph(TNodeValue)
    ///> parent=IDigraph(TNodeValue)
    template<class TNodeValue>
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
        // Description:	Add a new node to the Digraph without connecting it
        // Parameter: 	NodeValue val: A data value that is associated with the added node
        // Returns:   	NodeID:  A unique ID used to reference the added node
        // in further Digraph methods
        //************************************
        NodeID AddNode(const _In_ NodeValue& val)
        {
            m_adjList.insert(make_pair(++m_lastNodeId, MakePair(val, NodeSet())));

            return m_lastNodeId;
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
        // IStrategizer::IDigraph<TNodeValue>::GetRoots
        // Description:	Gets a list of all nodes that has an InDegree of zero.
        // Returns:   	std::vector<int>: A list of all nodes that has an InDegree of zero.
        //************************************
        std::vector<int> GetRoots() const
        {
            std::vector<int> roots;

            for each(auto possibleRoot in m_adjList)
            {
                bool hasZeroDegree = true;

                for each(auto otherNode in m_adjList)
                {
                    if(otherNode.second.second.count(possibleRoot.first))
                    {
                        hasZeroDegree = false;
                        break;
                    }
                }

                if(hasZeroDegree)
                {
                    roots.push_back(possibleRoot.first);
                }
            }
            
            return roots;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::SubGraphSubstitution
        // Description:	Replaces a sub-part of the IDigraph with the given TNodeValue provided.
        // Parameter: 	std::vector<int> p_subGraphIndexes: The indexes describing the sub-part to replace.
        // Parameter:   TNodeValue p_substitute: The TNodeValue to replace the sub-part with.
        //************************************      
        void SubGraphSubstitution(std::vector<int> p_subGraphIndexes, TNodeValue p_substitute)
        {
            std::vector<int>    m_parents;
            std::vector<int>    m_children;

            for(size_t i = 0; i < p_subGraphIndexes.size(); ++i)
            {
                std::vector<int> m_tempParents = GetParents(p_subGraphIndexes[i], m_parents);
                m_parents.insert(m_parents.end(), m_tempParents.begin(), m_tempParents.end());

                std::vector<int> m_tempChildren = GetChildren(p_subGraphIndexes[i], m_children);
                m_children.insert(m_children.end(), m_tempChildren.begin(), m_tempChildren.end());
            }

            NodeID m_nodeID = AddNode(p_substitute);

            for(size_t i = 0; i < m_parents.size(); ++i) AddEdge(m_parents[i], m_nodeID);
            for(size_t i = 0; i < m_children.size(); ++i) AddEdge(m_nodeID, m_children[i]);
            for(size_t i = 0; i < p_subGraphIndexes.size(); ++i) RemoveNode(p_subGraphIndexes[i]);
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
        
        bool IsSubGraphOf(AdjListDigraph<TNodeValue>& p_parentGraph, std::vector<int>& p_matchedIndexes)
        {
            std::vector<int> m_parentNodes;
            std::unordered_set<int> m_matchedNodes;

            for each (NodeID nodeID in p_parentGraph.GetNodes())
            {
                m_parentNodes.push_back(nodeID);
            }

            bool result = MatchNodesAndChildren(GetRoots(), m_parentNodes, p_parentGraph, m_matchedNodes);
            p_matchedIndexes.insert(p_matchedIndexes.begin(), m_matchedNodes.begin(), m_matchedNodes.end());
            return result;
        }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Lock
        // Description:	Locks the graph for exclusive read/write access by the caller thread
        // Returns:   	void
        //************************************
        virtual void Lock() { m_lock.lock(); }

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Unlock
        // Description:	Unlocks the graph acquisition by caller thread
        // Returns:   	void
        //************************************
        virtual void Unlock() { m_lock.unlock(); }        OBJECT_SERIALIZABLE(AdjListDigraph);
        OBJECT_MEMBERS(2 ,&m_lastNodeId, &m_adjList);

    private:
        ///> type=NodeID
        NodeID m_lastNodeId;
            ///> type=map(pair(NodeID,NodeEntry))
        Serialization::SMap<NodeID, NodeEntry> m_adjList;

        std::mutex m_lock;

        bool MatchNodesAndChildren(
        std::vector<int>& p_candidateNodes,
        std::vector<int>& p_parentNodes,
        AdjListDigraph<TNodeValue>& p_parentGraph,
        std::unordered_set<int>& p_matchedIndexes)
        {
            for each(int m_candidateNodeId in p_candidateNodes)
            {
                bool m_foundMatch = false;

                for each(int m_parentNodeId in p_parentNodes)
                {
                    if (p_matchedIndexes.find(m_parentNodeId) != p_matchedIndexes.end())
                    {
                        continue;
                    }

                    IComparable* m_candidateNode = dynamic_cast<IComparable*>(GetNode(m_candidateNodeId));
                    IComparable* m_parentNode = dynamic_cast<IComparable*>(p_parentGraph.GetNode(m_parentNodeId));

                    if (m_candidateNode->Compare(m_parentNode) == 0)
                    {
                        std::unordered_set<int> m_currentMatchedSubNodes;

                        if (MatchNodesAndChildren(GetChildren(m_candidateNodeId), p_parentGraph.GetChildren(m_parentNodeId), p_parentGraph, m_currentMatchedSubNodes))
                        {
                            m_currentMatchedSubNodes.insert(m_parentNodeId);

                            for each (int m_matchedSubNode in m_currentMatchedSubNodes)
                            {
                                if (p_matchedIndexes.find(m_matchedSubNode) == p_matchedIndexes.end())
                                {
                                    p_matchedIndexes.insert(m_matchedSubNode);
                                }
                            }

                            m_foundMatch = true;
                            break;
                        }
                    }
                }

                if(m_foundMatch == false)
                {
                    return false;
                }
            }

            return true;
        }

        std::vector<int> GetChildren(NodeID p_nodeIndex, std::vector<int> p_execluded = std::vector<int>()) const
        {
            if (m_adjList.count(p_nodeIndex) == 0)
                throw ItemNotFoundException(XcptHere);

            std::vector<int> m_children;

            for each(auto child in m_adjList.at(p_nodeIndex).second)
            {
                if(find(p_execluded.begin(), p_execluded.end(), child) == p_execluded.end())
                {
                    m_children.push_back(child);
                }
            }

            return m_children;
        }

        std::vector<int> GetParents(NodeID p_nodeIndex, std::vector<int> p_execluded = std::vector<int>()) const
        {
            if (m_adjList.count(p_nodeIndex) == 0)
                throw ItemNotFoundException(XcptHere);

            std::vector<int> m_parents;
            
            for each(auto parent in m_adjList)
            {
                if(find(p_execluded.begin(), p_execluded.end(), parent.first) == p_execluded.end()
                    && find(parent.second.second.begin(), parent.second.second.end(), p_nodeIndex) != parent.second.second.end())
                {
                    m_parents.push_back(parent.first);
                }
            }

            return m_parents;
        }    };
}

#endif
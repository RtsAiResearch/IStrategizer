///> [Serializable]
#ifndef ADJLISTDIGRAPH_H
#define ADJLISTDIGRAPH_H

#include "IDigraph.h"
#include "SMap.h"
#include "SPair.h"
#include "SSet.h"
#include "UserObject.h"

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
                throw ItemNotFoundException(XcptHere);

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
                throw ItemNotFoundException(XcptHere);

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
                throw ItemNotFoundException(XcptHere);

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
                throw ItemNotFoundException(XcptHere);

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

            for each (auto nodeEntry in m_adjList)
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
                throw ItemNotFoundException(XcptHere);

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
            std::vector<int>    m_temp;

            for(size_t i = 0; i < p_subGraphIndexes.size(); ++i)
            {
                m_temp = GetParents(p_subGraphIndexes[i], m_parents);
                m_parents.insert(m_parents.end(), m_temp.begin(), m_temp.end());

                m_temp = GetChildren(p_subGraphIndexes[i], m_children);
                m_children.insert(m_children.end(), m_temp.begin(), m_temp.end());
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

            for each (auto nodeEntry in m_adjList)
            {
                NodeSet& adjacents = nodeEntry.second.second;

                for each (auto adjNodeId in adjacents)
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

            for each (auto nodeEntry in m_adjList)
            {
                NodeSet& adjacents = nodeEntry.second.second;

                if (adjacents.empty())
                    leaves.insert(nodeEntry.first);
            }

            return leaves;
        }
        
        bool IsSubGraph(const AdjListDigraph<TNodeValue>& p_candidate, std::vector<int>& p_matchedIndexes, int& p_matchedCount) const
        {
            std::vector<int>    m_roots = p_candidate.GetRoots();
            std::vector<int>    m_matching;
            std::vector<int>    m_primaryMatched;
            std::vector<int>    m_candidateMatched;
            std::vector<int>    m_currentMatched;
            bool                m_match = false;

            m_candidateMatched.reserve(p_candidate.m_adjList.size());
            p_matchedIndexes.reserve(p_candidate.m_adjList.size());
            m_currentMatched.reserve(p_candidate.m_adjList.size());
            m_primaryMatched.reserve(m_adjList.size());

            for (unsigned i = 0; i < m_roots.size(); ++i)
            {
                for each(auto j in m_adjList)
                {
                    if (j.second.first == p_candidate.m_adjList.at(m_roots[i]).first)
                    {
                        m_candidateMatched.clear();
                        m_currentMatched.clear();
                        m_primaryMatched.clear();
                        m_match = false;

                        m_primaryMatched.push_back(j.first);
                        m_candidateMatched.push_back(m_roots[i]);

                        MatchPath(p_candidate, m_primaryMatched, m_candidateMatched, m_currentMatched, m_match);

                        if(m_match)
                        {
                            for (unsigned k = 0; k < m_currentMatched.size(); ++k)
                            {
                                if (find(p_matchedIndexes.begin(), p_matchedIndexes.end(), m_currentMatched[k]) == p_matchedIndexes.end())
                                {
                                    p_matchedIndexes.push_back(m_currentMatched[k]);
                                }
                            }

                            break;
                        }
                    }
                }
            }

            p_matchedCount = p_matchedIndexes.size();
            return (p_matchedCount > 0);
        }

        OBJECT_SERIALIZABLE(AdjListDigraph);
        OBJECT_MEMBERS(2 ,&m_lastNodeId, &m_adjList);

    private:
        ///> type=NodeID
        NodeID m_lastNodeId;
            ///> type=map(pair(NodeID,NodeEntry))
        Serialization::SMap<NodeID, NodeEntry> m_adjList;

        void MatchPath(AdjListDigraph<TNodeValue> p_candidate, std::vector<int>& p_primaryMatched, 
            std::vector<int>& p_candidateMatched, std::vector<int>& p_currentMatched, bool& p_match) const
        {
            if (p_candidateMatched.empty())
            {
                p_match = true;
            }

            int m_primaryIndex, m_candidateIndex;

            for (unsigned i = 0; i < p_candidateMatched.size() && !p_match; ++i)
            {
                m_candidateIndex = p_candidateMatched[i];

                for (unsigned j = 0; j < p_primaryMatched.size() && !p_match; ++j)
                {
                    m_primaryIndex = p_primaryMatched[j];

                    if (m_adjList.at(m_primaryIndex).first == p_candidate.m_adjList.at(m_candidateIndex).first)
                    {
                        p_currentMatched.push_back(m_primaryIndex);
                        p_candidateMatched.erase(p_candidateMatched.begin() + i);
                        p_primaryMatched.erase(p_primaryMatched.begin() + j);

                        std::vector<int> m_childrenIndexes = GetChildren(m_primaryIndex);
                        p_primaryMatched.insert(p_primaryMatched.begin(), m_childrenIndexes.begin(), m_childrenIndexes.end());

                        m_childrenIndexes.clear();
                        m_childrenIndexes = p_candidate.GetChildren(m_candidateIndex);
                        p_candidateMatched.insert(p_candidateMatched.begin(), m_childrenIndexes.begin(), m_childrenIndexes.end());

                        MatchPath(p_candidate, p_primaryMatched, p_candidateMatched, p_currentMatched, p_match);
                    }
                }
            }
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
        }
    };
}

#endif
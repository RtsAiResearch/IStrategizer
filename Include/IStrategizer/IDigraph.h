///> [Serializable]
#ifndef IDIGRAPH_H
#define IDIGRAPH_H

#include "IStrategizerException.h"
#include "SSet.h"
#include <queue>

namespace IStrategizer
{
    // Expose the minimum set of methods of a Digraph with which
    // any Digraph algorithm can be implemented

    ///> class=IDigraph(TNodeValue)
    template<class TNodeValue>
    class IDigraph
    {
    public:
        ///> alias=NodeID(unsigned)
        typedef unsigned NodeID;
        ///> alias=NodeValue(TNodeValue)
        typedef TNodeValue NodeValue;
        ///> alias=NodeSet(set(NodeID))
        typedef Serialization::SSet<NodeID> NodeSet;

        typedef std::vector<NodeID> NodeList;

        typedef std::queue<NodeID> NodeQueue;

        static const NodeID NullNodeID = 0;

        virtual ~IDigraph() {};

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::AddNode
        // Description:	Add a new node to the Digraph without connecting it
        // Parameter: 	NodeValue val: A data value that is associated with the added node
        // Returns:   	NodeID:  A unique ID used to reference the added node
        // in further Digraph methods
        //************************************
        virtual NodeID AddNode(const _In_ NodeValue& val) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::RemoveNode
        // Description:	Disconnect a node from the digraph and removes it
        // Parameter: 	NodeID id: Unique ID to identify the node
        // Returns:   	void
        //************************************
        virtual void RemoveNode(_In_ NodeID id)
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::AddEdge
        // Description:	Create a directed edge that goes from sourceNode to a destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	void
        //************************************
        virtual void AddEdge(_In_ NodeID sourceNodeId, _In_ NodeID destNodeId) 
            throw(ItemAlreadyExistsException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::RemoveEdge
        // Description:	Removes the edge that goes from sourceNode to destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	void
        //************************************
        virtual void RemoveEdge(_In_ NodeID sourceNodeId, _In_ NodeID destNodeId) 
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetNode
        // Description:	Get the value associated with the node
        // Parameter: 	NodeID id: Unique ID to identify the node
        // Returns:   	IStrategizer::NodeValue
        //************************************
        virtual NodeValue& GetNode(_In_ NodeID id) 
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetNodes
        // Description:	Returns a set of all node ids inside the digraph
        // Returns:   	NodeSet
        //************************************
        virtual NodeSet GetNodes() const = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Size
        // Description:	Returns the number of nodes inside the digraph
        // Returns:   	size_t
        //************************************
        virtual size_t Size() const = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Clear
        // Description:	Delete all graph nodes and edges
        // Returns:   	void
        //************************************
        virtual void Clear() = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::IsAdjacent
        // Description:	Check whether there is an edge that goes from sourceNode to destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	bool
        //************************************
        virtual bool IsAdjacent(_In_ NodeID sourceNodeId, _In_ NodeID destNodeId) const 
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetAdjacentNodes
        // Description:	Get all nodes that has an edge going from sourceNode to it
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Returns:   	NodeSet: A set of all node ids adjacent to sourceNode
        //************************************
        virtual const NodeSet& GetAdjacentNodes(_In_ NodeID sourceNodeId) const 
            throw(ItemNotFoundException) = 0;
        
        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetRoots
        // Description:	Gets a list of all nodes that has an InDegree of zero.
        // Returns:   	std::vector<int>: A list of all nodes that has an InDegree of zero.
        //************************************
        virtual std::vector<int> GetRoots() const = 0;
        
        //************************************
        // IStrategizer::IDigraph<TNodeValue>::SubGraphSubstitution
        // Description:	Replaces a sub-part of the IDigraph with the given TNodeValue provided.
        // Parameter: 	std::vector<int> p_subGraphIndexes: The indexes describing the sub-part to replace.
        // Parameter:   TNodeValue p_substitute: The TNodeValue to replace the sub-part with.
        //************************************      
        virtual void SubGraphSubstitution(std::vector<int> p_subGraphIndexes, TNodeValue p_substitute) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetOrphanNodes
        // Description:	Get all nodes that do not have an edge going from any node to them
        // Returns:   	NodeSet: A set of all orphan node ids
        //************************************
        virtual NodeSet GetOrphanNodes() const = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetLeafNodes
        // Description:	Get all nodes that do not have an edge going from them to any node
        // Returns:   	NodeSet
        //************************************
        virtual NodeSet GetLeafNodes() const = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Lock
        // Description:	Locks the graph for exclusive read/write access by the caller thread
        // Returns:   	void
        //************************************
        virtual void Lock() = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::Unlock
        // Description:	Unlocks the graph acquisition by caller thread
        // Returns:   	void
        //************************************
        virtual void Unlock() = 0;
    };
}

#endif // IDIGRAPH_H
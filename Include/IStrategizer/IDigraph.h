///> [Serializable]
#ifndef IDIGRAPH_H
#define IDIGRAPH_H

#include "IStrategizerException.h"
#include "SSet.h"

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

        virtual ~IDigraph() {};

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::AddNode
        // Description:	Add a new node to the Digraph without connecting it
        // Parameter: 	NodeValue val: A data value that is associated with the added node
        // Returns:   	NodeID:  A unique ID used to reference the added node
        // in further Digraph methods
        //************************************
        virtual NodeID AddNode(NodeValue val) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::RemoveNode
        // Description:	Disconnect a node from the digraph and removes it
        // Parameter: 	NodeID id: Unique ID to identify the node
        // Returns:   	void
        //************************************
        virtual void RemoveNode(NodeID id)
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::AddEdge
        // Description:	Create a directed edge that goes from sourceNode to a destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	void
        //************************************
        virtual void AddEdge(NodeID sourceNodeId, NodeID destNodeId) 
            throw(ItemAlreadyExistsException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::RemoveEdge
        // Description:	Removes the edge that goes from sourceNode to destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	void
        //************************************
        virtual void RemoveEdge(NodeID sourceNodeId, NodeID destNodeId) 
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetNode
        // Description:	Get the value associated with the node
        // Parameter: 	NodeID id: Unique ID to identify the node
        // Returns:   	IStrategizer::NodeValue
        //************************************
        virtual NodeValue GetNode(NodeID id) 
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetNodes
        // Description:	Returns a set of all node ids inside the digraph
        // Returns:   	NodeSet
        //************************************
        virtual NodeSet GetNodes() const = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::IsAdjacent
        // Description:	Check whether there is an edge that goes from sourceNode to destNode
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Parameter: 	NodeID destNodeId: Unique ID to identify destNode
        // Returns:   	bool
        //************************************
        virtual bool IsAdjacent(NodeID sourceNodeId, NodeID destNodeId) const 
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetAdjacentNodes
        // Description:	Get all nodes that has an edge going from sourceNode to it
        // Parameter: 	NodeID sourceNodeId: Unique ID to identify sourceNode
        // Returns:   	NodeSet: A set of all node ids adjacent to sourceNode
        //************************************
        virtual NodeSet GetAdjacentNodes(NodeID sourceNodeId) const 
            throw(ItemNotFoundException) = 0;

        //************************************
        // IStrategizer::IDigraph<TNodeValue>::GetOrphanNodes
        // Description:	Get all nodes that do not have an edge going from any node to them
        // Returns:   	NodeSet: A set of all orphan node ids
        //************************************
        virtual NodeSet GetOrphanNodes() const = 0;
    };
}

#endif // IDIGRAPH_H
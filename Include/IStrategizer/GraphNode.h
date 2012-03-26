///> [Serializable]
#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <vector>
using namespace std;

#ifndef USEROBJECT_H
	#include "UserObject.h"
#endif
using namespace Serialization;

#define DISCONNECTED	0
#define CONNECTED		1

///> class=GraphEdge(TAnnotation)
template<class TAnnotation>
struct GraphEdge : public UserObject
{
    ///> type=TAnnotation
	TAnnotation	edgeAnnotation;
    ///> type=int
	int			isConnected;

	inline GraphEdge() : isConnected(DISCONNECTED) {}
	inline GraphEdge(TAnnotation p_edgeAnnotation, int p_isConnected = DISCONNECTED) : edgeAnnotation(p_edgeAnnotation), isConnected(p_isConnected) {}

	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string  	TypeName() 	{ return "GraphEdge"; }
	int     	TypeSize()  { return sizeof(GraphEdge); }
	UserObject* Prototype() { return new GraphEdge; }	
	
protected:
    void InitializeAddressesAux() { AddMemberAddress(2, &edgeAnnotation, &isConnected); }
	//----------------------------------------------------------------------------------------------
};

///> class=GraphNode(TValue,TAnnotation)
template<class TValue, class TAnnotation>
class GraphNode : public UserObject
{
public:
    ///> alias=Edges(vector(GraphEdge(TAnnotation)*))
	typedef SVector < GraphEdge<TAnnotation>* > Edges;
private:
    ///> type=Edges
	Edges	_outEdges;
    ///> type=Edges
    Edges	_inEdges;
    ///> type=TValue
    TValue	_value;
    ///> type=int
    int		_inDegree;
    ///> type=int
    int		_outDegree;

public:
	inline				GraphNode(): _inDegree(0), _outDegree(0) {}
	inline Edges		OutEdges() const { return _outEdges; }
	inline Edges		InEdges() const { return _inEdges; }
	inline int			InDegree() const { return _inDegree; }
	inline int			OutDegree() const { return _outDegree; }
	inline int			IsConnected(int p_node) const { return _outEdges[p_node]->isConnected; }
	inline int			IncreaseInDegree(){ return ++_inDegree; }
	inline int			DecreaseInDegree(){ return --_inDegree; }
	inline int			IncreaseOutDegree(){ return ++_outDegree; }
	inline int			DecreaseOutDegree(){ return --_outDegree; }
	inline TValue		Value() const { return _value; }
	inline void			Value(TValue p_val) { _value = p_val; }
	inline TAnnotation	GetAnnotation(int p_index) { return _outEdges[p_index]->edgeAnnotation; }
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string  	TypeName() 	{ return "GraphNode"; }
	int     	TypeSize()  { return sizeof(GraphNode); }
	UserObject* Prototype() { return new GraphNode; }	
	
protected:
    void InitializeAddressesAux()
    {
        AddMemberAddress(5,
            &_outEdges,
            &_inEdges,
            &_value,
            &_inDegree,
            &_outDegree);
    }
public:
	//---------------------------------------------------------------------------
	GraphNode(int p_size, TValue p_val = TValue()): _inDegree(0), _outDegree(0), _value(p_val) 
	{
		_outEdges = Edges(p_size, NULL);
		_inEdges = Edges(p_size, NULL);

		for (unsigned i = 0; i < _outEdges.size(); ++i)
		{
			_outEdges[i] = new GraphEdge<TAnnotation>();
			_inEdges[i] = new GraphEdge<TAnnotation>();
		}
	}
	//---------------------------------------------------------------------------
	bool Connect(int p_node, TAnnotation p_ann) 
	{
		if (_outEdges[p_node]->isConnected) return false;

		_outEdges[p_node]->isConnected = CONNECTED;
		_outEdges[p_node]->edgeAnnotation = p_ann;
		_outDegree++;

		return true;
	}
	//---------------------------------------------------------------------------
	bool Disconnect(int p_node) 
	{
		if (_outEdges[p_node]->isConnected)
		{
			delete _outEdges[p_node];
			_outEdges[p_node] = new GraphEdge<TAnnotation>();
			_outDegree--;
			return true;
		}
		
		return false;
	}
	//--------------------------------------------------------------------------
	bool Equals(GraphNode<TValue, TAnnotation> p_rhs)
	{
		IComparable* m_lhs = static_cast<IComparable*>(_value);
		IComparable* m_rhs = static_cast<IComparable*>(p_rhs._value);

		return !m_lhs->Compare(m_rhs);
	}
	//--------------------------------------------------------------------------
	void UpdateNode()
	{
		_outDegree = 0;
		_inDegree = 0;

		for(int i = 0; i < _outEdges.size(); ++i) _outDegree += (_outEdges[i]->isConnected) ? CONNECTED : DISCONNECTED;
	}
	//--------------------------------------------------------------------------
	void SetParent(int p_parentIndex,  int p_connectionVal, TAnnotation p_annotation = TAnnotation()) 
	{
		_inEdges[p_parentIndex]->isConnected = p_connectionVal; 
		_inEdges[p_parentIndex]->edgeAnnotation = p_annotation;
		_inDegree += (p_connectionVal == CONNECTED) ? 1 : -1;
	}
	//--------------------------------------------------------------------------
	void RemoveNode(unsigned p_node)
	{
		if (_inEdges[p_node]->isConnected) _inDegree--;
		if (_outEdges[p_node]->isConnected) _outDegree--;

		_outEdges.erase(_outEdges.begin() + p_node);
		_inEdges.erase(_inEdges.begin() + p_node);
	}
	//--------------------------------------------------------------------------
	vector<int> GetChildren(vector<int> p_execluded = vector<int>())
	{
		vector<int> m_children;

		for (unsigned i = 0; i < _outEdges.size(); ++i)
		{
			if (_outEdges[i]->isConnected && find(p_execluded.begin(), p_execluded.end(), i) == p_execluded.end())
				m_children.push_back(i);
		}

		return m_children;
	}
	//--------------------------------------------------------------------------
	vector<int> GetParents(vector<int> p_execluded = vector<int>())
	{
		vector<int> m_parents;

		for (unsigned i = 0; i < _inEdges.size(); ++i)
		{
			if (_inEdges[i]->isConnected && find(p_execluded.begin(), p_execluded.end(), i) == p_execluded.end())
				m_parents.push_back(i);
		}

		return m_parents;
	}
	//--------------------------------------------------------------------------
	void AddNode(unsigned p_index)
	{
		if (p_index == _outEdges.size())
		{
			_outEdges.resize(_outEdges.size() + 1); _inEdges.resize(_inEdges.size() + 1);
			_outEdges[p_index] = new GraphEdge<TAnnotation>();
			_inEdges[p_index] = new GraphEdge<TAnnotation>();
		}
	}
};

#endif GRAPHNODE_H
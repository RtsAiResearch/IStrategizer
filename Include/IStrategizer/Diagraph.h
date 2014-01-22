///> [Serializable]
#ifndef DIAGRAPH_H
#define DIAGRAPH_H

#include <functional>

#include <vector>

#ifndef GRAPHNODE_H
#include "GraphNode.h"
#endif

#ifndef ICOMPARABLE_H
#include "IComparable.h"
#endif

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

namespace IStrategizer
{
    ///> class=Diagraph(TValue,TAnnotation)
    template<class TValue, class TAnnotation>
    class Diagraph : public Serialization::UserObject
    {
        OBJECT_SERIALIZABLE(Diagraph);

    private:
        void MatchPath(Diagraph<TValue, TAnnotation> p_candidate, std::vector<int>& p_primaryMatched, 
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

                    if (_adjacencyMatrix[m_primaryIndex]->Equals(*p_candidate[m_candidateIndex]))
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

    protected:
        ///> type=vector(GraphNode(TValue,TAnnotation)*)
        Serialization::SVector< GraphNode<TValue, TAnnotation>* >    _adjacencyMatrix;
    protected:
        void InitializeAddressesAux() { AddMemberAddress(1, &_adjacencyMatrix); }
    public:
        Diagraph() {}
        Diagraph(unsigned p_nodesCount)
        {
            _adjacencyMatrix.resize(p_nodesCount);

            for(unsigned i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                _adjacencyMatrix[i] = new GraphNode<TValue, TAnnotation>(p_nodesCount);
            }
        }
        //---------------------------------------------------------------------------
        inline GraphNode<TValue, TAnnotation>* operator[] (int p_index) const { return _adjacencyMatrix[p_index]; }
        //---------------------------------------------------------------------------
        inline GraphNode<TValue, TAnnotation>* At (int p_index) const { return _adjacencyMatrix[p_index]; }
        //---------------------------------------------------------------------------
        inline int            IsConnected(int p_node1, int p_node2) const { return _adjacencyMatrix[p_node1]->IsConnected(p_node2); }
        //---------------------------------------------------------------------------
        inline size_t        Size() const { return _adjacencyMatrix.size(); }
        //---------------------------------------------------------------------------
        std::vector<int> GetInDegree() const
        {
            std::vector<int> m_inDegree = std::vector<int>(_adjacencyMatrix.size(), 0);

            for (size_t i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                m_inDegree[i] = _adjacencyMatrix[i]->InDegree();
            }

            return m_inDegree;
        }
        //---------------------------------------------------------------------------
        bool Connect(int p_node1, int p_node2, TAnnotation p_annotation)
        {
            if (_adjacencyMatrix[p_node1]->Connect(p_node2, p_annotation))
            {
                _adjacencyMatrix[p_node2]->SetParent(p_node1, CONNECTED, p_annotation);
                return true;
            }

            return false;
        }
        //-------------------------5--------------------------------------------------
        bool Disconnect(int p_node1, int p_node2)
        {
            if (_adjacencyMatrix[p_node1]->Disconnect(p_node2))
            {
                _adjacencyMatrix[p_node2]->SetParent(p_node1, DISCONNECTED);

                return true;
            }

            return false;
        }
        //---------------------------------------------------------------------------
        std::vector<int>    GetRoots() const
        {
            std::vector<int> roots;

            for(unsigned i = 0 ; i < _adjacencyMatrix.size() ; i++)
            {
                if(!_adjacencyMatrix[i]->InDegree())
                {
                    roots.push_back(i);
                }
            }

            return roots;
        }
        //----------------------------------------------------------------------------------------------
        std::vector<int> GetLeaves() const
        {
            std::vector<int> leaves;
            for(unsigned i = 0; i < _adjacencyMatrix.size(); i++)
                if(!_adjacencyMatrix[i]->OutDegree())
                    leaves.push_back(i);
            return leaves;
        }
        //---------------------------------------------------------------------------
        size_t AddNode(TValue p_value, size_t p_index)
        {
            for (unsigned i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                _adjacencyMatrix[i]->AddNode(p_index);
            }

            if (_adjacencyMatrix.size() == p_index)
            {
                _adjacencyMatrix.resize(_adjacencyMatrix.size() + 1);
                _adjacencyMatrix[p_index] = new GraphNode<TValue, TAnnotation>(_adjacencyMatrix.size(), p_value);
            }
            else
            {
                _adjacencyMatrix[p_index]->Value(p_value);
            }

            // Returns the added node index
            return _adjacencyMatrix.size() - 1;
        }
        //---------------------------------------------------------------------------
        TAnnotation GetConnectionAnnotation(int p_node1, int p_node2) { return _adjacencyMatrix[p_node1]->GetAnnotation(p_node2); }
        //---------------------------------------------------------------------------
        std::vector<TAnnotation> TotalInAnnotations(int p_node) const
        {
            std::vector<TAnnotation> m_totalInAnnotations;

            for (size_t i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                if (IsConnected(i, p_node))
                {
                    m_totalInAnnotations.push_back(_adjacencyMatrix[i][p_node]->edgeAnnotation);
                }
            }

            return m_totalInAnnotations;
        }
        //---------------------------------------------------------------------------
        std::vector<TAnnotation> TotalOutAnnotations(int p_node) const
        {
            std::vector<TAnnotation> m_totalOutAnnotations;

            for (size_t i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                if (IsConnected(p_node, i))
                {
                    m_totalOutAnnotations.push_back(_adjacencyMatrix[p_node][i]->edgeAnnotation);
                }
            }

            return m_totalOutAnnotations;
        }
        //---------------------------------------------------------------------------
        void RemoveNode(unsigned p_node)
        { 
            for (unsigned i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                _adjacencyMatrix[i]->RemoveNode(p_node);
            }

            _adjacencyMatrix.erase(_adjacencyMatrix.begin() + p_node);
        }
        //---------------------------------------------------------------------------
        bool IsSubGraph(Diagraph<TValue, TAnnotation> p_candidate, std::vector<int>& p_matchedIndexes, int& p_matchedCount) const
        {
            std::vector<int> m_roots = p_candidate.GetRoots();
            std::vector<int>    m_matching;
            std::vector<int>    m_primaryMatched;
            std::vector<int>    m_candidateMatched;
            std::vector<int>    m_currentMatched;
            bool        m_match = false;

            m_candidateMatched.reserve(p_candidate._adjacencyMatrix.size());
            p_matchedIndexes.reserve(p_candidate._adjacencyMatrix.size());
            m_currentMatched.reserve(p_candidate._adjacencyMatrix.size());
            m_primaryMatched.reserve(_adjacencyMatrix.size());

            for (unsigned i = 0; i < m_roots.size(); ++i)
            {
                for (unsigned j = 0; j < _adjacencyMatrix.size(); ++j)
                {
                    if (_adjacencyMatrix[j]->Equals(*p_candidate[m_roots[i]]))
                    {
                        m_candidateMatched.clear();
                        m_currentMatched.clear();
                        m_primaryMatched.clear();
                        m_match = false;

                        m_primaryMatched.push_back(j);
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
        //---------------------------------------------------------------------------
        std::vector<int> GetParents(int p_nodeIndex, std::vector<int> p_execluded = std::vector<int>()) const
        {
            std::vector<int> m_parents;

            for (size_t i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                if (_adjacencyMatrix[i]->IsConnected(p_nodeIndex) &&
                    find(p_execluded.begin(), p_execluded.end(), i) == p_execluded.end())
                {
                    m_parents.push_back(i);
                }
            }

            return m_parents;
        }
        //---------------------------------------------------------------------------
        std::vector<int> GetChildren(int p_nodeIndex, std::vector<int> p_execluded = std::vector<int>()) const
        {
            std::vector<int> m_children;

            for (unsigned i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                if (_adjacencyMatrix[p_nodeIndex]->IsConnected(i) &&
                    find(p_execluded.begin(), p_execluded.end(), i) == p_execluded.end())
                {
                    m_children.push_back(i);
                }
            }

            return m_children;
        }
        //---------------------------------------------------------------------------
        //void SubGraphSubstitution(std::vector<int> p_subGraphIndexes, TValue p_substitute)
        //{
        //    map<int, TAnnotation>    m_parents;
        //    map<int, TAnnotation>    m_children;
        //    std::vector<TAnnotation>        m_pAnnotations;
        //    std::vector<TAnnotation>        m_cAnnotations;
        //    std::vector<int>                m_temp;
        //    int                        i;

        //    sort(p_subGraphIndexes.begin(), p_subGraphIndexes.end(), less<int>());

        //    for (i = 0; i < p_subGraphIndexes.size(); ++i)
        //    {
        //        m_temp.clear();
        //        m_temp = GetChildren(p_subGraphIndexes[i]);
        //        
        //        for(int j = 0; j < m_temp.size(); ++j) m_children[j] += GetConnectionAnnotation(i, j);

        //        m_temp.clear();
        //        m_temp = GetParents(p_subGraphIndexes[i]);

        //        for(int j = 0; j < m_temp.size(); ++j) m_parents[j] += GetConnectionAnnotation(j, i);
        //    }

        //    AddNode(p_substitute, Size());

        //    for (i = 0; i < m_parents.size(); ++i)    Connect(m_parents[i], Size() - 1, m_pAnnotations[i]);
        //    for (i = 0; i < m_children.size(); ++i) Connect(Size() - 1, m_children[i], m_cAnnotations[i]);

        //    for (int i = 0; i < p_subGraphIndexes.size(); ++i)
        //    {
        //        RemoveNode( p_subGraphIndexes[i] - i );
        //    }
        //}
        //---------------------------------------------------------------------------
        void UpdateDiagraph()
        {
            // FIXME: each node should know it's parents
            for (int i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                _adjacencyMatrix[i]->UpdateNode();
            }

            for (int i = 0; i < _adjacencyMatrix.size(); ++i)
            {
                for (int j = 0; i < _adjacencyMatrix.size(); ++j)
                {
                    if (i != j && _adjacencyMatrix[i]->IsConnected(j))
                    {
                        _adjacencyMatrix[j]->IncreaseInDegree();
                    }
                }
            }
        }
    };
}

#endif    // DIAGRAPH_H

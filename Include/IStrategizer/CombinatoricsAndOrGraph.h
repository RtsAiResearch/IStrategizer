#ifndef COMBINATORICSANDORGRAPH_H
#define COMBINATORICSANDORGRAPH_H

#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <list>

#ifndef METADATA_H
	#include "MetaData.h"
#endif
using namespace MetaData;

#ifndef DELEGATE_H
	#include "Delegate.h"
#endif

#ifndef PREDICATE_H
	#include "Predicate.h"
#endif

using namespace std;

typedef pair<int, int> NodeType;
typedef vector<NodeType> Group;
typedef vector<Group> Level;

template<class TObserver>
class CombinatoricsAndOrGraph
{
private:
    vector<Level>   m_levels;
    list<NodeType>  m_path;
//----------------------------------------------------------------------------------------------
protected:
    //bool EvaluateAnd( int p_levelIdx, int p_groupIdx );
    //bool EvaluateOr( int p_levelIdx, int p_parentGroupIdx, int p_parentNodeIdx );
//----------------------------------------------------------------------------------------------
public:
    vector<Level>& Levels() { return m_levels; }
    list<NodeType>& Path()  { return m_path; }

    Predicate<TObserver>* EvaluatePath;

    CombinatoricsAndOrGraph() : EvaluatePath(NULL) {} 
    //----------------------------------------------------------------------------------------------
    void Evaluate()
    {
        if(!m_levels.empty())
            EvaluateOr(0, -1, -1);
    }
    //----------------------------------------------------------------------------------------------
    bool EvaluateOr( int p_levelIdx , int p_parentGroupIdx, int p_parentNodeIdx)
    {
        // Base case: node is leaf
        if(p_levelIdx == m_levels.size())
        {
            return OnEvaluatePath(p_levelIdx - 1, p_parentGroupIdx, p_parentNodeIdx);
        }
        // Inductive case: there are more levels to check
        else
        {
            Level &currentLevel = m_levels[p_levelIdx];
            bool result = false;
            for(int groupIdx = 0; groupIdx < currentLevel.size(); ++groupIdx)
            {
                result |= EvaluateAnd(p_levelIdx, groupIdx);
            }

            return result;
        }
    }
    //----------------------------------------------------------------------------------------------
    bool EvaluateAnd( int p_levelIdx, int p_groupIdx )
    {
        Group &currentGroup = m_levels[p_levelIdx][p_groupIdx];
        for(int nodeIdx = 0; nodeIdx < currentGroup.size(); ++nodeIdx)
        {
            m_path.push_back(m_levels[p_levelIdx][p_groupIdx][nodeIdx]);
            if(!EvaluateOr(p_levelIdx + 1, p_groupIdx, nodeIdx))
            {
                m_path.pop_back();
                return false;
            }
            m_path.pop_back();
        }
        return true;
    }
    //----------------------------------------------------------------------------------------------
    bool OnEvaluatePath( int p_levelIdx, int p_groupIdx, int p_nodeIdx )
    {
        /* FIXME: evaluation may use the parameters */
        return EvaluatePath->Call();
    }
    //----------------------------------------------------------------------------------------------
    ~CombinatoricsAndOrGraph()
    {
        if(EvaluatePath != NULL)
            delete EvaluatePath;
    }
};



#endif	// COMBINATORICSANDORGRAPH_H
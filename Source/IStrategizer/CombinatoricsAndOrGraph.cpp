//#include "COMBINATORICSANDORGRAPH_H.h"
//
//
////----------------------------------------------------------------------------------------------
//void CombinatoricsAndOrGraph::Evaluate()
//{
//    if(!m_levels.empty())
//        EvaluateOr(0, -1, -1);
//}
//
////----------------------------------------------------------------------------------------------
//bool CombinatoricsAndOrGraph::EvaluateOr( int p_levelIdx , int p_parentGroupIdx, int p_parentNodeIdx)
//{
//    // Base case: node is leaf
//    if(p_levelIdx == m_levels.size())
//    {
//        if(OnEvaluatePath(p_levelIdx - 1, p_parentGroupIdx, p_parentNodeIdx))
//        {
//            OnProcessPath();
//            return true;
//        }
//        return false;
//    }
//    // Inductive case: there are more levels to check
//    else
//    {
//        Level &currentLevel = m_levels[p_levelIdx];
//        bool result = false;
//        for(int groupIdx = 0; groupIdx < currentLevel.size(); ++groupIdx)
//        {
//            result |= EvaluateAnd(p_levelIdx, groupIdx);
//        }
//
//        return result;
//    }
//}
//
////----------------------------------------------------------------------------------------------
//bool CombinatoricsAndOrGraph::EvaluateAnd( int p_levelIdx, int p_groupIdx )
//{
//    Group &currentGroup = m_levels[p_levelIdx][p_groupIdx];
//    for(int nodeIdx = 0; nodeIdx < currentGroup.size(); ++nodeIdx)
//    {
//        m_path.push_back(m_levels[p_levelIdx][p_groupIdx][nodeIdx]);
//        if(!EvaluateOr(p_levelIdx + 1, p_groupIdx, nodeIdx))
//        {
//            m_path.pop_back();
//            return false;
//        }
//        m_path.pop_back();
//    }
//    return true;
//}
//
////----------------------------------------------------------------------------------------------
//void CombinatoricsAndOrGraph::OnProcessPath()
//{
//    ProcesPath();
//}
//
////----------------------------------------------------------------------------------------------
//bool CombinatoricsAndOrGraph::OnEvaluatePath( int p_levelIdx, int p_groupIdx, int p_nodeIdx )
//{
//    /* FIXME: evaluation may use the parameters */
//    return EvaluatePath();
//}
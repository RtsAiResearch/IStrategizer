#include "ConditionEx.h"

using namespace IStrategizer;
using namespace std;

//----------------------------------------------------------------------------------------------
bool ConditionEx::Equals(const Expression* p_rhs) const
{
    if(p_rhs->ExpressionType() != EXPRESSION_Leaf)
        return false;

    const ConditionEx* m_rhs = static_cast<const ConditionEx*>(p_rhs);

    if(m_rhs == nullptr)
        return false;

    bool m_equal = (_conditionType == m_rhs->_conditionType) &&
                    (m_params.size() == m_rhs->m_params.size());

    if(!m_equal)
        return false;

    for (PlanStepParameters::const_iterator i = m_params.begin(), j = m_rhs->m_params.begin();
         m_equal && i != m_params.end(); ++i, ++j)
    {
        if ((*i).first == PARAM_Amount && (*j).first == PARAM_Amount)
        {
            // Always return true if the amount of the parameter is amount as
            // amount can be consumed and does not require exact match.
            m_equal &= true;

            /*if ((*i).second == DONT_CARE || (*j).second == DONT_CARE)
            {
                m_equal &= true;
            }
            else
            {
                m_equal &= ((*i).first == (*j).first) && ((*i).second <= (*j).second);
            }*/
        }
        else
        {
            if (((*i).first == (*j).first) && ((*i).second == DONT_CARE || (*j).second == DONT_CARE))
            {
                m_equal &= true;
            }
            else
            {
                m_equal &= ((*i).first == (*j).first) && ((*i).second == (*j).second);
            }
        }
    }
    
    return m_equal;
}
//------------------------------------------------------------------------------
bool ConditionEx::PartiallyEqualsAux(const Expression* p_rhs, MatchSide p_anchor, vector<pair<Expression*,Expression*>>& p_matchedLeafs) const
{
    if(p_rhs->ExpressionType() == EXPRESSION_Leaf)
    {
        if(Equals(p_rhs))
        {
            for(vector<pair<Expression*,Expression*>>::iterator itr = p_matchedLeafs.begin(); 
                itr != p_matchedLeafs.end();
                ++itr)
            {
                if(p_rhs == itr->first || p_rhs == itr->second || this == itr->first || this == itr->second)
                    return false;
            }

            if(p_anchor == MATCHSIDE_Left)
            { 
                p_matchedLeafs.push_back(make_pair(const_cast<ConditionEx*>(this), const_cast<Expression*>(p_rhs)));
            }
            else
            {
                p_matchedLeafs.push_back(make_pair(const_cast<Expression*>(p_rhs), const_cast<ConditionEx*>(this)));
            }

            return true;
        }

        return false;
    }
    else
    {
        return p_rhs->PartiallyEqualsAux(this, MATCHSIDE_Right, p_matchedLeafs);
    }
}
//----------------------------------------------------------------------------------------------------------------
void ConditionEx::Copy(IClonable* p_dest)
{
    Expression::Copy(p_dest);

    ConditionEx* m_dest = static_cast<ConditionEx*>(p_dest);

    m_dest->_conditionType = _conditionType;
    m_dest->m_params.insert(m_params.begin(), m_params.end());
    m_dest->_playerType = _playerType;
}

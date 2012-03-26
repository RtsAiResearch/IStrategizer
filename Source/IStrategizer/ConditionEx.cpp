#include "ConditionEx.h"

void ConditionEx::InitializeAddressesAux()
{
    Expression::InitializeAddressesAux();
    AddMemberAddress(3,
        &_conditionType,
        &_conditionParameters,
        &_playerType);
}
//----------------------------------------------------------------------------------------------
bool ConditionEx::Equals(const Expression* p_rhs) const
{
    if(p_rhs->ExpressionType() != EXPRESSION_Leaf)
        return false;

    const ConditionEx* m_rhs = static_cast<const ConditionEx*>(p_rhs);

    if(m_rhs == NULL)
        return false;

	bool m_equal = (_conditionType == m_rhs->_conditionType) &&
					(_conditionParameters.size() == m_rhs->_conditionParameters.size());

    if(!m_equal)
        return false;

	for (PlanStepParameters::const_iterator i = _conditionParameters.begin(), j = m_rhs->_conditionParameters.begin();
		 m_equal && i != _conditionParameters.end(); ++i, ++j)
	{
		if ((*i).first == PARAM_Amount && (*j).first == PARAM_Amount)
		{
			m_equal &= ((*i).first == (*j).first) && ((*i).second <= (*j).second);
		}
		else
		{
			m_equal &= ((*i).first == (*j).first) && ((*i).second == (*j).second);
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

            ConditionEx* me = const_cast<ConditionEx*>(this);
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

	m_dest->_conditionType        = _conditionType;
	m_dest->_conditionParameters  = _conditionParameters;
	m_dest->_playerType           = _playerType;
}
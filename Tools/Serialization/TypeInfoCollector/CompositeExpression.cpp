#include "CompositeExpression.h"
#include <algorithm>
#include <cassert>

//----------------------------------------------------------------------------------------------
void CompositeExpression::InitializeAddressesAux()
{
    Expression::InitializeAddressesAux();
    AddMemberAddress(2,
        &_expressions,
        &_shortCircuit);
}
//----------------------------------------------------------------------------------------------
bool CompositeExpression::AddExpression(Expression* p_expression)
{
    vector<Expression*>::iterator m_where;
    m_where = find(_expressions.begin(), _expressions.end(), p_expression);

    if(m_where == _expressions.end())
    {
        _expressions.push_back(p_expression);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------------------------
bool CompositeExpression::RemoveExpression(Expression* p_expression)
{
    vector<Expression*>::iterator m_where;
    m_where = find(_expressions.begin(), _expressions.end(), p_expression);

    if(m_where != _expressions.end())
    {
        _expressions.erase(m_where);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------------------------
bool CompositeExpression::Equals(const Expression* p_rhs) const
{
    bool m_equals = false;

    if(p_rhs->ExpressionType() != EXPRESSION_Composite)
        return false;

    const CompositeExpression* m_rhs = static_cast<const CompositeExpression*>(p_rhs);
    
    if(m_rhs == NULL)
        return false;

    // assuming that the condition checking is not deep (this is not a CFG for checking equivalence)
    if(m_rhs->Expressions().size() != _expressions.size())
        return false;

    vector<Expression*>::const_iterator rhsItr = m_rhs->Expressions().begin();
    for(vector<Expression*>::const_iterator lhsItr = _expressions.begin();
        lhsItr != _expressions.end();
        lhsItr++ , rhsItr++)
    {
        m_equals = (*lhsItr)->Equals(*rhsItr);
        if(!m_equals)
            return false;
    }

    return true;
}
//----------------------------------------------------------------------------------------------
bool CompositeExpression::PartiallyEqualsAux(const Expression* p_rhs, MatchSide p_matchSide, vector<pair<Expression*,Expression*>>& p_matchedLeafs) const
{
    bool m_equals = false;
	for(int lhsIdx = 0; lhsIdx < _expressions.size(); ++lhsIdx)
	{
		m_equals |= _expressions[lhsIdx]->PartiallyEqualsAux(p_rhs, p_matchSide, p_matchedLeafs);
	}

	return m_equals;
}
//----------------------------------------------------------------------------------------------
Expression* CompositeExpression::operator[](int p_index)
{
    int m_seed = 0;
    Expression* m_exp = GetTermAux(m_seed, p_index);
    
    // Term index out of range
    assert(m_exp != NULL);
    
    return m_exp;
}
//----------------------------------------------------------------------------------------------
Expression* CompositeExpression::At(int p_index)
{
    int m_seed = 0;
    Expression* m_exp = GetTermAux(m_seed, p_index);

    // Term index out of range
    assert(m_exp != NULL);

    return m_exp;
}
//----------------------------------------------------------------------------------------------
Expression* CompositeExpression::GetTermAux(int p_currentIdex, int p_targetIndex)
{
    CompositeExpression* m_compositeExp = NULL;

    for(vector<Expression*>::iterator itr;
        itr != _expressions.end();
        itr++)
    {

        if((*itr)->ExpressionType() == EXPRESSION_Leaf)
        {
            // Base case
            if(p_currentIdex == p_targetIndex)
            {       
                return (*itr);
            }
            else
            {
                p_currentIdex++;
            }
        }
        else
        {
            m_compositeExp = NULL;
            m_compositeExp = static_cast<CompositeExpression*>(*itr);
            assert(m_compositeExp != NULL);

            return m_compositeExp->GetTermAux(p_currentIdex, p_targetIndex);
        }
    }

    // term index out of range
    return NULL;
}
//----------------------------------------------------------------------------------------------
void CompositeExpression::Initialize(CompositeExpression* p_object)
{
	Expression::Initialize(p_object);

	p_object->_expressions.resize(_expressions.size());
	for (int i = 0; i < _expressions.size(); ++i)
	{
		p_object->_expressions[i] = _expressions[i]->Clone();
	}
	
	_shortCircuit = p_object->_shortCircuit;
}
#include "CompositeExpression.h"
#include <algorithm>
#include <cassert>

using namespace IStrategizer;
using namespace std;

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
    
    if(m_rhs == nullptr)
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
    for(size_t lhsIdx = 0; lhsIdx < _expressions.size(); ++lhsIdx)
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
    _ASSERTE(m_exp != nullptr);
    
    return m_exp;
}
//----------------------------------------------------------------------------------------------
Expression* CompositeExpression::At(int p_index)
{
    int m_seed = 0;
    Expression* m_exp = GetTermAux(m_seed, p_index);

    // Term index out of range
    _ASSERTE(m_exp != nullptr);

    return m_exp;
}
//----------------------------------------------------------------------------------------------
void CompositeExpression::Clear()
{
    for(int i = 0, size = _expressions.size(); i < size; ++i)
    {
        delete _expressions[i];
        _expressions[i] = nullptr;
    }

    _expressions.Clear();
}
//----------------------------------------------------------------------------------------------
Expression* CompositeExpression::GetTermAux(int p_currentIdex, int p_targetIndex)
{
    CompositeExpression* m_compositeExp = nullptr;

    for(vector<Expression*>::iterator itr = _expressions.begin();
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
            m_compositeExp = nullptr;
            m_compositeExp = static_cast<CompositeExpression*>(*itr);
            _ASSERTE(m_compositeExp != nullptr);

            return m_compositeExp->GetTermAux(p_currentIdex, p_targetIndex);
        }
    }

    // term index out of range
    return nullptr;
}
//----------------------------------------------------------------------------------------------
void CompositeExpression::Copy(IClonable* p_dest)
{
    Expression::Copy(p_dest);

    CompositeExpression* m_dest = static_cast<CompositeExpression*>(p_dest);

    m_dest->_expressions.resize(_expressions.size());
    for (size_t i = 0; i < _expressions.size(); ++i)
    {
        m_dest->_expressions[i] = _expressions[i] ? static_cast<Expression*>(_expressions[i]->Clone()) : nullptr;
    }
    
    _shortCircuit = m_dest->_shortCircuit;
}

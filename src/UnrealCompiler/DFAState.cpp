#include "DFAState.h"
#include "AlphabetFactory.h"

//////////////////////////////////////////////////////////////////////////
DFAState::DFAState(int p_stateId, int p_tokenTypeId)
{
    m_stateId   = p_stateId;
    m_tokenTypeId = p_tokenTypeId;
}

//////////////////////////////////////////////////////////////////////////
DFAState::DFAState(int p_stateId, int p_tokenTypeId, const vector< pair<string, int> >& p_transitionFunction)
{
    m_stateId   = p_stateId;
    m_tokenTypeId = p_tokenTypeId;

    for(vector< pair<string, int> >::const_iterator itr = p_transitionFunction.begin();
        itr != p_transitionFunction.end();
        itr++)
    {
        AddTransition(itr->first, itr->second);
    }
}

//////////////////////////////////////////////////////////////////////////
int DFAState::Consume(char p_inputChar)
{
    if(m_transitionFunction.find(p_inputChar) != m_transitionFunction.end())
    {
        return m_transitionFunction[p_inputChar];
    }

    return Error;
}

//////////////////////////////////////////////////////////////////////////
void DFAState::AddTransition(const string& p_input, int p_newStateId)
{
    string p_expandedInput;
    g_AlphabetFactory.GetAlphabet(p_expandedInput, p_input);

    for(string::iterator itr = p_expandedInput.begin();
        itr != p_expandedInput.end();
        itr++)
    {
        m_transitionFunction[*itr] = p_newStateId;
    }
}

#include "DFA.h"
#include "Toolbox.h"
#include <fstream>

//////////////////////////////////////////////////////////////////////////
DFA::DFA(int p_startState, int p_errorState, const vector<DFAState*>& p_states)
    : m_startState(p_startState), m_errorState(p_errorState), m_states(p_states) {}

//////////////////////////////////////////////////////////////////////////
void DFA::Reset()
{
    m_currentState = m_startState;
}

//////////////////////////////////////////////////////////////////////////
int DFA::Move(char p_inputChar)
{
    int newState = m_states[m_currentState]->Consume(p_inputChar);

    if(newState == Error)
    {
        m_currentState = m_errorState;
    }
    else
    {
        m_currentState = newState;
    }

    return m_states[m_currentState]->GetTokenTypeId();
}

DFA::~DFA()
{
    Toolbox::MemoryClean(m_states);
}


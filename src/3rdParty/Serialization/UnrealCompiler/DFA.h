#ifndef DFA_H
#define DFA_H

#include <set>
#include <map>
#include "DFAState.h"
using namespace std;

class DFA
{
    int                 m_startState;
    int                 m_errorState;
    int                 m_currentState;
    vector<DFAState*>   m_states;               
public:
    DFA(int p_startState, int p_errorState,  const vector<DFAState*>& p_states);
    void Reset();
    int Move(char p_input);
    ~DFA();
};

#endif // DFA_H

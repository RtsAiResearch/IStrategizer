#ifndef DFASTATE_H
#define DFASTATE_H

#include <vector>
#include <map>
#include "Token.h"
using namespace std;

const int Error = -1;

class DFAState
{
    int             m_stateId;
    int             m_tokenTypeId;
    map<char, int>  m_transitionFunction;
    
    void AddTransition( const string& p_input, int p_newStateId);
public:

    int GetTokenTypeId() const { return m_tokenTypeId; }
    int GetStateId() const { return m_stateId; }

    DFAState(int p_stateId, int p_tokenTypeId);
    DFAState(int p_stateId, int p_tokenTypeId,  const vector< pair<string, int> >& p_transitionFunction);
    int Consume(char p_inputChar);
};

#endif // DFASTATE_H
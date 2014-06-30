#ifndef DFASTATE_H
#define DFASTATE_H

#include <vector>
#include <map>
#include "Token.h"

const int Error = -1;

class DFAState
{
    int             m_stateId;
    int             m_tokenTypeId;
    std::map<char, int>  m_transitionFunction;
    
    void AddTransition(const std::string& p_input, int p_newStateId);
public:

    int GetTokenTypeId() const { return m_tokenTypeId; }
    int GetStateId() const { return m_stateId; }

    DFAState(int p_stateId, int p_tokenTypeId);
    DFAState(int p_stateId, int p_tokenTypeId,  const std::vector< std::pair<std::string, int> >& p_transitionFunction);
    int Consume(char p_inputChar);
};

#endif // DFASTATE_H

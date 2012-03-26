#include "ParseTable.h"
#include <algorithm>
#include <cassert>
#include "SDTScheme.h"

void ParseTable::Initialize()
{
    ComputeFirst();
    ComputeFollow();
    ComputeParseTable();
}

//////////////////////////////////////////////////////////////////////////
void ParseTable::ComputeFirst()
{
    for(set<string>::iterator itr = m_grammar->NonTerminals.begin();
        itr != m_grammar->NonTerminals.end();
        itr++)
    {
        ComputeFirstAux0(*itr);
    }
}
//////////////////////////////////////////////////////////////////////////
void ParseTable::ComputeFirstAux0( string& p_symbol )
{
    // Base case: first is already computed
    if(!m_first[p_symbol].empty())
    {
        return;
    }
    // Base case: symbol is a terminal, so FIRST(symbol) = {symbol}
    else if(m_grammar->Terminals.find(p_symbol) != m_grammar->Terminals.end())
    {
        m_first[p_symbol].insert(p_symbol);
    }
    // Inductive case: first not computed for symbol, that is FIRST(symbol) = {}
    else
    {
        for(vector< ProductionBody >::iterator itr = m_grammar->Productions[p_symbol].begin();
            itr != m_grammar->Productions[p_symbol].end();
            itr++)
        {
            ComputeFirstAux1(p_symbol, itr->Production);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ParseTable::ComputeFirstAux1(string& p_productionHead, ProductionType& p_productionBody )
{
    for(ProductionBody::ProductionItr itr = p_productionBody.begin();
        itr != p_productionBody.end();
        itr++)
    {
        if(*itr == SemanticActionMarker)
            continue;

        ComputeFirstAux0(*itr);
        
        // no epsilon productions
        if(m_first[*itr].find(Epsilon) == m_first[*itr].end())
        {
            // add all production body symbols
            m_first[p_productionHead].insert(m_first[*itr].begin(), m_first[*itr].end());
            return;
        }
        else
        {
            // add all production body symbols except epsilon
            for(set<string>::iterator copyItr = m_first[*itr].begin();
                copyItr != m_first[*itr].end();
                copyItr++)
            {
                if(*copyItr == Epsilon || *copyItr == SemanticActionMarker)
                    continue;

                m_first[p_productionHead].insert(*copyItr);
            }
        }
    }   

    // If all FIRST(production body symbols) contains epsilon then add epsilon to FIRST(production head symbol)
    m_first[p_productionHead].insert(Epsilon);
}
//////////////////////////////////////////////////////////////////////////
void ParseTable::GetFirst(ProductionBody::ProductionItr p_symbolsBegin, ProductionBody::ProductionItr p_symbolsEnd, set<string>& p_first)
{
    for(ProductionBody::ProductionItr itr = p_symbolsBegin;
        itr != p_symbolsEnd;
        itr++)
    {
        if(*itr == SemanticActionMarker)
            continue;

        ComputeFirstAux0(*itr);

        // no epsilon productions
        if(m_first[*itr].find(Epsilon) == m_first[*itr].end())
        {
            // add all production body symbols
            p_first.insert(m_first[*itr].begin(), m_first[*itr].end());
            return;
        }
        else
        {
            // add all production body symbols except epsilon
            for(set<string>::iterator copyItr = m_first[*itr].begin();
                copyItr != m_first[*itr].end();
                copyItr++)
            {
                if(*copyItr == Epsilon || *copyItr == SemanticActionMarker)
                    continue;

                 p_first.insert(*copyItr);
            }
        }
    }   

    // If all FIRST(production body symbols) contains epsilon then add epsilon to FIRST(production head symbol)
    p_first.insert(Epsilon);
}

//////////////////////////////////////////////////////////////////////////
void ParseTable::ComputeFollow()
{
    m_follow[m_grammar->Start].insert(RightEndMarker);

    for(set<string>::iterator itr = m_grammar->NonTerminals.begin();
        itr != m_grammar->NonTerminals.end();
        itr++)
    {
        ComputeFollowAux0(*itr);
    }
}
//////////////////////////////////////////////////////////////////////////
void ParseTable::ComputeFollowAux0( string& p_symbol )
{
    // FOLLOW(symbol) is already computed
    if (!(m_follow[p_symbol].empty() ||
       (p_symbol == m_grammar->Start && m_follow[p_symbol].size() == 1)))
        return;

    ProductionBody::ProductionItr   where;
    set<string>::iterator           whereEpsilon;
    set<string>                     first;

    for(set<string>::iterator nonTerminalItr = m_grammar->NonTerminals.begin();
        nonTerminalItr != m_grammar->NonTerminals.end();
        nonTerminalItr++)
    {
        // ignores searching in production of the form A -> aAb where a and b can be null
        if(*nonTerminalItr == p_symbol)
            continue;

        for(vector< ProductionBody >::iterator productionItr = m_grammar->Productions[*nonTerminalItr].begin();
            productionItr != m_grammar->Productions[*nonTerminalItr].end();
            productionItr++)
        {
            
            where = find(productionItr->Production.begin(), productionItr->Production.end(), p_symbol);

            // production does not contain symbol
            if(where == productionItr->Production.end())
                continue;
            
            int idx = where - productionItr->Production.begin();

            // A -> aB OR
            // A -> aBsssss... where s is a semantic action
            // FOLLOW(B) = FOLLOW(A)
            if(idx == productionItr->Production.size() - 1 ||
               count(where + 1, productionItr->Production.end(), SemanticActionMarker) == productionItr->Production.size() - idx - 1)
            {
                ComputeFollowAux0(*nonTerminalItr);
                m_follow[p_symbol].insert(m_follow[*nonTerminalItr].begin(), m_follow[*nonTerminalItr].end());
            }
            // A -> aBb
            // A -> aBsb
            else
            {
                // FIXME: it is assumed that the grammar will not contain production as A -> aAbA
                // Find first non-semantic action symbol
                for(where = where + 1;
                    where != productionItr->Production.end();
                    where++)
                {
                    if(*where != SemanticActionMarker && *where != p_symbol)
                    {
                        break;
                    }
                }
                
                assert(where != productionItr->Production.end());

                first.clear();
                GetFirst(where, productionItr->Production.end(), first);

                // A -> aBb and FIRST(b) contains e
                // FOLLOW(B) = FIRST(b) - {e} + FOLLOW(A)
                whereEpsilon = first.find(Epsilon);
                if(whereEpsilon != first.end())
                {
                    first.erase(whereEpsilon);
                    m_follow[p_symbol].insert(first.begin(), first.end());

                    ComputeFollowAux0(*nonTerminalItr);
                    m_follow[p_symbol].insert(m_follow[*nonTerminalItr].begin(), m_follow[*nonTerminalItr].end());
                }
                // A -> aBb and FIRST(b) does not contain e
                // FOLLOW(B) = FIRST(b)
                else
                {
                    m_follow[p_symbol].insert(first.begin(), first.end());
                }
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ParseTable::ComputeParseTable()
{
    set<string> first;

    for(set<string>::iterator nonTerminalItr = m_grammar->NonTerminals.begin();
        nonTerminalItr != m_grammar->NonTerminals.end();
        nonTerminalItr++)
    {
        // foreach production A -> b
        for(vector< ProductionBody >::iterator productionItr = m_grammar->Productions[*nonTerminalItr].begin();
            productionItr != m_grammar->Productions[*nonTerminalItr].end();
            productionItr++)
        {
            first.clear();
            GetFirst(productionItr->Production.begin(), productionItr->Production.end(), first);
            // [1] foreach terminal a in FIRST(b), add A -> b to M[A, a]
            for(set<string>::iterator terminalItr = first.begin();
                terminalItr != first.end();
                terminalItr++)
            {
                if(*terminalItr == Epsilon)
                    continue;

                // if the entry M[A, a] is not empty then this is not LL(1) grammar
                assert(m_parseTable[*nonTerminalItr].Row.find(*terminalItr) == m_parseTable[*nonTerminalItr].Row.end());
                m_parseTable[*nonTerminalItr].Row[*terminalItr] = productionItr;
            }

            // if e is in FIRST(b)
            if(first.find(Epsilon) != first.end())
            {
                // [1] foreach terminal c in FOLLOW(A), add A -> b to M[A, c]
                for(set<string>::iterator terminalItr = m_follow[*nonTerminalItr].begin();
                    terminalItr != m_follow[*nonTerminalItr].end();
                    terminalItr++)
                {
                    // if the entry M[A, a] is not empty then this is not LL(1) grammar
                    assert(m_parseTable[*nonTerminalItr].Row.find(*terminalItr) == m_parseTable[*nonTerminalItr].Row.end());

                    m_parseTable[*nonTerminalItr].Row[*terminalItr] = productionItr;
                }
            }
        }

        ComputeSynchSet(*nonTerminalItr);
    }
}
//////////////////////////////////////////////////////////////////////////
bool ParseTable::GetProductionAt(const string& p_nonTerminal, const string& p_terminal, ProductionType& p_production, int& p_relativeIdx)
{
    // it is illogical to miss a non-terminal in the parse table, this means an unsuccessful parse table generation
    assert(m_parseTable.find(p_nonTerminal) != m_parseTable.end());

    ParseTableRow::Iterator where = m_parseTable[p_nonTerminal].Row.find(p_terminal);
    if(where == m_parseTable[p_nonTerminal].Row.end())
    {
        return false;
    }
    else
    {
        vector<ProductionBody>::iterator itr = where->second;
        p_production.reserve(itr->Production.size());
        p_production.insert(p_production.begin(), itr->Production.begin(), itr->Production.end());

        p_relativeIdx = itr - m_grammar->Productions[p_nonTerminal].begin();
        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
void ParseTable::ComputeSynchSet(const string& p_nonTerminal)
{
    for(set<string>::iterator itr = m_follow[p_nonTerminal].begin();
        itr != m_follow[p_nonTerminal].end();
        itr++)
    {
        m_synch[p_nonTerminal].insert(*itr);
    }
}
//////////////////////////////////////////////////////////////////////////
ParseTable::~ParseTable()
{
}

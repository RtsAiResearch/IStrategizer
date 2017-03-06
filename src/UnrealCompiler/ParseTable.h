#ifndef PARSETABLE_H
#define PARSETABLE_H

#include <map>
#include <set>
#include <string>
#include "Grammar.h"
using namespace std;

struct ParseTableRow
{
    map< string, vector<ProductionBody>::iterator > Row;
    typedef map< string, vector<ProductionBody>::iterator >::iterator Iterator;
};

#define RightEndMarker "$"
#define Synch "synch"

class ParseTable
{
    map<string, set<string> >   m_first;
    map<string, set<string> >   m_follow;
    map<string, set<string> >   m_synch;
    Grammar*                    m_grammar;
    // <Production Head, <Terminal, Production Body Pointer>>
    map<string, ParseTableRow>  m_parseTable;

    void ComputeFirst();
    void ComputeFollow();
    void ComputeParseTable();
    void ComputeFirstAux0( const string& p_symbol );
    void ComputeFirstAux1( const string& p_productionHead, ProductionType& p_productionBody);
    void GetFirst(ProductionBody::ProductionItr p_symbolsBegin, ProductionBody::ProductionItr p_symbolsEnd, set<string>& p_first);
    void ComputeFollowAux0(const string& p_symbol );
    void ComputeSynchSet(const string& p_nonTerminal);
public:
    ParseTable(Grammar* p_grammar) : m_grammar(p_grammar) { Initialize(); }
    ~ParseTable();
    void Initialize();
    bool GetProductionAt(const string& p_nonTerminal,const string& p_terminal, ProductionType& p_production, int& p_relativeIdx);
};

#endif // PARSETABLE_H

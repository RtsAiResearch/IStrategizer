#ifndef PREDICTIVEPARSER_H
#define PREDICTIVEPARSER_H

#include <stack>
using namespace std;

#include "LexicalAnalyzer.h"
#include "AbstractParser.h"
#include "ErrorData.h"

class ParseTable;

class PredictiveParser : public AbstractParser
{
    LexicalAnalyzer*        m_scanner;
    ParseTable*             m_parseTable;
    stack<string>           m_stack;
    stack<ParseTreeNode*>   m_nodesStack;
    void Initialize();

public:
    void Reset();
    void Parse();
    void ReadNextSymbol(Token*& p_currentToken, string& p_symbol);
    void OnUnexpectedToken( const string& p_expected, const Token* p_found );
    void OnSyntaxError( const Token* p_token );

    PredictiveParser(LexicalAnalyzer* p_scanner) : m_scanner(p_scanner) { Initialize(); }
    ~PredictiveParser();

};

#endif // PREDICTIVEPARSER_H

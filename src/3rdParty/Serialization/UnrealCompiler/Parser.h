#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "LexicalAnalyzer.h"
#include "Event.h"
#include "AbstractParser.h"
using namespace std;

class Parser;
typedef Event<Parser> ParserEvent;

class Parser : public AbstractParser
{
    Token*              m_currentToken;
    string     m_output;
    LexicalAnalyzer*    m_scanner;
    
    void GetNextToken();
    void Match(string p_terminal, ParseTreeNode* p_parent);
    void StmtSequence(ParseTreeNode* p_parent);
    void SimpleExp(ParseTreeNode* p_parent);
    void Term(ParseTreeNode* p_parent);
    void Exp(ParseTreeNode* p_parent);
    void Statement(ParseTreeNode* p_parent);      
    void Factor(ParseTreeNode* p_parent);     

public:
    void            Parse();
    string          Output() const;
    void            Reset();
    
    Parser(LexicalAnalyzer* p_scanner);
    ~Parser();
};

#endif // PARSER_H

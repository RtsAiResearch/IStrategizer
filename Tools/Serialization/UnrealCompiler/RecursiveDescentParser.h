#ifndef RECURSIVEDESCENTPARSER_H
#define RECURSIVEDESCENTPARSER_H

#include "LexicalAnalyzer.h"
#include "TreeNode.h"
#include "AbstractParser.h"

class RecursiveDescentParser : public AbstractParser
{
    LexicalAnalyzer*    m_scanner;
    ParseTreeNode*      m_parseTree;
public:
    RecursiveDescentParser(LexicalAnalyzer* p_scanner) : m_scanner(p_scanner) {}
    void            Reset();
    void            Parse();
};

#endif // RECURSIVEDESCENTPARSER_H
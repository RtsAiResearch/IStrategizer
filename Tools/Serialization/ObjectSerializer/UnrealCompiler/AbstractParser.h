#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H

#include "ParseTree.h"
#include "Event.h"
#include "Toolbox.h"
#include "Grammar.h"

class AbstractParser;
typedef Event<AbstractParser> AbstractParserEvent;

class AbstractParser
{
protected:
    ParseTreeNode*      m_parseTree;
    Grammar*            m_grammar;
public:
    AbstractParserEvent Fail;
    virtual void    Reset() = 0;
    virtual void    Parse() = 0;
    ParseTreeNode*  ParseTree() const { return m_parseTree; }
    Grammar* Grammar() const { return m_grammar; }
    AbstractParser() : m_parseTree(NULL) {}
    ~AbstractParser() { Toolbox::MemoryClean(m_parseTree); }
};

#endif // ABSTRACTPARSER_H
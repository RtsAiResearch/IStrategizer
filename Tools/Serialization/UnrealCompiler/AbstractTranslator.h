#ifndef ABSTRACTTRANSLATOR_H
#define ABSTRACTTRANSLATOR_H

#include "Event.h"
#include "ParseTree.h"

class AbstractTranslator;

typedef Event<AbstractTranslator> AbstractTranslatorEvent;

class AbstractTranslator
{
protected:
    ParseTreeNode*      m_annotatedParseTreeRoot;
public:
    AbstractTranslatorEvent Fail;
    virtual void    Translate(ParseTreeNode* p_parseTreeRoot) = 0;
    ParseTreeNode*  AnnotatedParseTree() const { return m_annotatedParseTreeRoot; }
    AbstractTranslator() : m_annotatedParseTreeRoot(NULL) {}
    ~AbstractTranslator() { if(m_annotatedParseTreeRoot != NULL) delete m_annotatedParseTreeRoot; }
};

#endif // ABSTRACTTRANSLATOR_H._H
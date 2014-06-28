#ifndef SYNTAXDIRECTEDTRANSLATOR_H
#define SYNTAXDIRECTEDTRANSLATOR_H

#include "AbstractTranslator.h"
#include "Grammar.h"

class SyntaxDirectedTranslator : public AbstractTranslator
{
protected:
    Grammar* m_grammar;
public:
    void Translate(ParseTreeNode* p_parseTreeRoot);
    SyntaxDirectedTranslator(Grammar* p_grammar) : m_grammar(p_grammar) {}
    void TranslateAux( ParseTreeNode* p_node );
    void ExecuteAction( ParseTreeNode* p_node, SemanticAction& p_action );
    void BindAttributes(ParseTreeNode* p_node, vector<LateBindingParameter>& p_params, vector<pair<AttributeName, AttributeSet*> >& p_bindingTable);
};

#endif // SYNTAXDIRECTEDTRANSLATOR_H

#ifndef TYPEDECLARATIONPARSER_H
#define TYPEDECLARATIONPARSER_H

#include <map>
using namespace std;

#ifndef LEXICALANALYZER_H
#include "LexicalAnalyzer.h"
#endif

#ifndef ABSTRACTPARSER_H
    #include "AbstractParser.h"
#endif

#ifndef TYPENODE_H
	#include "TypeNode.h"
#endif

class TypeDeclarationParser : public AbstractParser
{
    LexicalAnalyzer*        m_scanner;
    Token*                  m_currentToken;
    TypeNode*               m_typeGraph;
    map<string, DataType>   m_builtInTypes;

public:
    void        Reset();
    void        Parse();
    TypeNode*   TypeGraph() { return m_typeGraph; }
    TypeDeclarationParser(LexicalAnalyzer* p_scanner);
protected:
    void Declaration(TypeNode* p_parent);
    bool Match(string p_terminal);
    void ParanParam(TypeNode* p_parent);
    void Pointer(TypeNode* p_parent);
    void ParamList(TypeNode* p_parent);
    void Param(TypeNode* p_parent);
    void SetNodeType( string p_terminal, TypeNode* p_parent );
};

#endif // TYPEDECLARATIONPARSER_H_H

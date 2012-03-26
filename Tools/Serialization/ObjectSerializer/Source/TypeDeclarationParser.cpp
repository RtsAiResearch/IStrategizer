#ifndef  TYPEDECLARATIONPARSER_H
#include "TypeDeclarationParser.h"
#endif

TypeDeclarationParser::TypeDeclarationParser(LexicalAnalyzer* p_scanner) : m_scanner(p_scanner) 
{
    m_builtInTypes["bool"]      = DTYPE_Bool;
    m_builtInTypes["char"]      = DTYPE_Char;
    m_builtInTypes["short"]     = DTYPE_Short;
    m_builtInTypes["int"]       = DTYPE_Int;
    m_builtInTypes["float"]     = DTYPE_Float;
    m_builtInTypes["double"]    = DTYPE_Double;
    m_builtInTypes["array"]     = DTYPE_Array;
    m_builtInTypes["vector"]    = DTYPE_Vector;
    m_builtInTypes["map"]       = DTYPE_Map;
    m_builtInTypes["set"]       = DTYPE_Set;
    m_builtInTypes["pair"]      = DTYPE_Pair;
    m_builtInTypes["string"]    = DTYPE_String;
}
//----------------------------------------------------------------------------------------------
void TypeDeclarationParser::Reset()
{
    m_scanner->Reset();
}
//----------------------------------------------------------------------------------------------
void TypeDeclarationParser::Parse()
{
    m_currentToken = m_scanner->GetNextToken();

    if(!m_currentToken->IsEOF())
        Declaration(NULL);
}
//----------------------------------------------------------------------------------------------
void TypeDeclarationParser::SetNodeType( string p_terminal, TypeNode* p_parent )
{
    if(m_builtInTypes.find(p_terminal) == m_builtInTypes.end())
    {
        p_parent->Type = DTYPE_UserDefined;
        p_parent->UserDefinedType = p_terminal;
    }
    else
    {
        p_parent->Type = m_builtInTypes[p_terminal];
    }
}
//----------------------------------------------------------------------------------------------
bool TypeDeclarationParser::Match(string p_terminal)
{
    if(g_TokenTypesMap[m_currentToken->TypeId]->Name == p_terminal)
    {
        m_currentToken = m_scanner->GetNextToken();
        return true;
    }
    else
    {
        Fail( this, NULL );
        return false;
    }
}
//----------------------------------------------------------------------------------------------
void TypeDeclarationParser::Declaration(TypeNode* p_parent)
{
    TypeNode* node = NULL;
    Token* currentToken = NULL;

    node = new TypeNode(DTYPE_Undefined);
    if(p_parent == NULL)
    {
        m_typeGraph = node;
    }
    else
    {
        p_parent->TemplateArguments.push_back(node);
    }

    currentToken = m_currentToken;
    if(!Match("identifier"))
    {
        Fail(this, NULL);
        return;
    }
    SetNodeType(currentToken->Value->Lexeme, node);

    ParanParam(node);
    Pointer(node);
}

//----------------------------------------------------------------------------------------------
void TypeDeclarationParser::ParanParam(TypeNode* p_parent)
{
    if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "(")
    {
        if(!Match("("))
        {
            Fail(this, NULL);
            return;
        }

        Param(p_parent);
        ParamList(p_parent);

        if(!Match(")"))
        {
            Fail(this, NULL);
            return;
        }
    }
}
//----------------------------------------------------------------------------------------------
void TypeDeclarationParser::Pointer(TypeNode* p_parent)
{
    if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "*")
    {
        if(!Match("*"))
        {
            Fail(this, NULL);
            return;
        }
        p_parent->Indirection = true;
    }
}
//----------------------------------------------------------------------------------------------
void TypeDeclarationParser::ParamList(TypeNode* p_parent)
{
    if(g_TokenTypesMap[m_currentToken->TypeId]->Name == ",")
    {
        if(!Match(","))
        {
            Fail(this, NULL);
            return;
        }

        Param(p_parent);
        ParamList(p_parent);
    }
}
//----------------------------------------------------------------------------------------------
void TypeDeclarationParser::Param(TypeNode* p_parent)
{
    if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "identifier")
        Declaration(p_parent);
    else if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "number")
    {
        Token* currentToken = NULL;
        currentToken = m_currentToken;
        if(!Match("number"))
        {
            Fail(this, NULL);
            return;
        }

        const char* lexeme = currentToken->Value->Lexeme.c_str();
        p_parent->Children.push_back(TypeChild(atoi(lexeme)));
    }
}

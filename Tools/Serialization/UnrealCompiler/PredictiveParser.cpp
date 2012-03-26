#include "PredictiveParser.h"
#include "ParseTable.h"
#include "CompilerSettings.h"
#include "SDTScheme.h"
#include "ErrorData.h"

void PredictiveParser::Initialize()
{
    m_grammar = new SDTScheme;
    m_grammar->Initialize(g_CompilerSettings.GrammarDefinitionPath());

    m_parseTable = new ParseTable(m_grammar);
}
//////////////////////////////////////////////////////////////////////////
void PredictiveParser::Reset()
{
    if(m_parseTree != NULL)
    {
        delete m_parseTree;
        m_parseTree = NULL;
    }

    m_nodesStack    = stack<ParseTreeNode*>();
    m_stack         = stack<string>();
}

//////////////////////////////////////////////////////////////////////////
void PredictiveParser::ReadNextSymbol(Token*& p_currentToken, string& p_symbol)
{
    Token* token = m_scanner->GetNextToken();
    
    p_currentToken = token;

    if(token->IsEOF())
    {
        p_symbol = RightEndMarker;
    }
    else
    {
        if(g_TokenTypesMap[token->TypeId]->IsError)
        {
            ReadNextSymbol(p_currentToken, p_symbol);
        }
        else
        {
            p_symbol = g_TokenTypesMap[token->TypeId]->Name;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void PredictiveParser::Parse()
{
    Token*          currentToken = NULL;
    string          inputSymbol;
    string          stackSymbol = "";
    ParseTreeNode*  stackNode   = NULL;
    ParseTreeNode*  currentNode = NULL;
    ProductionType  production;
    int             productionRelativeIdx;

    assert(m_parseTree == NULL);

    ReadNextSymbol(currentToken, inputSymbol);

    m_stack.push(RightEndMarker);
    m_stack.push(m_grammar->Start);

    m_nodesStack.push(new ParseTreeNode(m_grammar->Start, new AttributeSet));
    m_parseTree = m_nodesStack.top();

    do
    {
        stackSymbol = m_stack.top();
        m_stack.pop();

        if(!m_nodesStack.empty())
        {
            stackNode   = m_nodesStack.top();
            m_nodesStack.pop();
        }

        if (m_grammar->Terminals.find(stackSymbol) != m_grammar->Terminals.end() ||
            stackSymbol == RightEndMarker)
        {
            if(stackSymbol == inputSymbol)
            {
                // FIXME: it is assumed that all terminal lex values are integers
                stackNode->Value()->LexVal.Value = atoi(currentToken->Value->Lexeme.c_str());

                ReadNextSymbol(currentToken, inputSymbol);
            }
            else
            {
                // Error: unexpected token
                // simulate we match a similar input symbol as the stack symbol
                OnUnexpectedToken(stackSymbol, currentToken);
            }
        }
        else
        {
            production.clear();
            if(m_parseTable->GetProductionAt(stackSymbol, inputSymbol, production, productionRelativeIdx))
            {
                stackNode->Value()->ProductionRelativeIdx = productionRelativeIdx;
                for(ProductionBody::ReverseProductionItr itr = production.rbegin();
                    itr != production.rend();
                    itr++)
                {
                    // skip actions now, for later processing by the translator

                    currentNode = new ParseTreeNode(*itr, new AttributeSet);
                    stackNode->AddChild(currentNode);

                    if(*itr == SemanticActionMarker)
                        continue;

                    if(*itr != Epsilon)
                    {
                        m_stack.push(*itr);
                        m_nodesStack.push(currentNode);
                    }
                }

                stackNode->ReverseChildren();
            }
            else
            {
                OnSyntaxError(currentToken);
                return;
            }
        }
    }
    while(stackSymbol != RightEndMarker);
}
//////////////////////////////////////////////////////////////////////////
void PredictiveParser::OnUnexpectedToken(const string& p_expected, const Token* p_found )
{
    ErrorData* errorData;
    vector<string> parameters;

    parameters.push_back(p_expected);

    // case if, else, then, ), <, etc...
    if(p_found->Value->Lexeme.empty())
        parameters.push_back(g_TokenTypesMap[p_found->TypeId]->Name);
    // case identifier, number, etc...
    else
        parameters.push_back(p_found->Value->Lexeme);

    errorData = new ErrorData(ERROR_UnexpectedToken, parameters, p_found->Value->Row, p_found->Value->Column);

    Fail(this, errorData);
}
//////////////////////////////////////////////////////////////////////////
void PredictiveParser::OnSyntaxError(const Token* p_token )
{
    ErrorData* errorData;
    vector<string> parameters;

    if(p_token->Value->Lexeme.empty())
    {
        parameters.push_back(g_TokenTypesMap[p_token->TypeId]->Name);
    }
    else
    {
        parameters.push_back(p_token->Value->Lexeme);
    }
    errorData = new ErrorData(ERROR_SyntaxError, parameters, p_token->Value->Row, p_token->Value->Column);


    Fail(this, errorData);
}
//////////////////////////////////////////////////////////////////////////
PredictiveParser::~PredictiveParser()
{
    Toolbox::MemoryClean(m_parseTable);
    Toolbox::MemoryClean(m_grammar);
}

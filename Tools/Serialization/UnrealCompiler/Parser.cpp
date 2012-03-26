#include "Parser.h"
#include "ErrorData.h"
#include <cassert>

Parser::Parser(LexicalAnalyzer* p_scanner)
{
    m_scanner = p_scanner;
	m_currentToken = NULL;
}

//////////////////////////////////////////////////////////////////////////
void Parser::Reset()
{
    if(m_parseTree != NULL)
    {
        delete m_parseTree;
        m_parseTree = NULL;
    }

    //m_recognizedTokens.clear();
}
//////////////////////////////////////////////////////////////////////////
void Parser::Parse()
{
    assert(m_parseTree == NULL);
    GetNextToken();
    StmtSequence(NULL);
}

//////////////////////////////////////////////////////////////////////////
string Parser::Output() const
{
    return m_output;
}
//////////////////////////////////////////////////////////////////////////
void Parser::GetNextToken()
{
	m_currentToken = m_scanner->GetNextToken();
	
	if (m_currentToken == NULL)
		m_output += "** currentToken = NULL ** \n";
}

//////////////////////////////////////////////////////////////////////////
void Parser::Match(string p_terminal, ParseTreeNode* p_parent)
{	
    assert(p_parent != NULL);

	if (p_terminal == g_TokenTypesMap[m_currentToken->TypeId]->Name)
	{
		GetNextToken();
        p_parent->AddChild(new ParseTreeNode(p_terminal));
	}
	else
	{
		//Error
		m_output += ">> Error in matching terminal << \n";
	}

}
//////////////////////////////////////////////////////////////////////////
void Parser::StmtSequence(ParseTreeNode* p_parent)
{
	if (m_currentToken == NULL)
		return;

    ParseTreeNode* current = NULL;

    if(p_parent == NULL)
    {
        m_parseTree = new ParseTreeNode("stmt-sequence");
        current   = m_parseTree;
    }
    else
    {
        current = p_parent->AddChild(new ParseTreeNode("stmt-sequence"));
    }

	Statement(current);

	while(m_currentToken && g_TokenTypesMap[m_currentToken->TypeId]->Name == ";")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		Statement(current);
	}
}
//////////////////////////////////////////////////////////////////////////
void Parser::SimpleExp(ParseTreeNode* p_parent)
{
    assert(p_parent != NULL);

	if (m_currentToken == NULL)
		return;

    ParseTreeNode* current = NULL;

    current = p_parent->AddChild(new ParseTreeNode("simple-exp"));
	Term(current);

	while(m_currentToken && 
        (g_TokenTypesMap[m_currentToken->TypeId]->Name == "+" ||
        g_TokenTypesMap[m_currentToken->TypeId]->Name == "-") )
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		Term(current);
	}
}
//////////////////////////////////////////////////////////////////////////
void Parser::Term(ParseTreeNode* p_parent)
{
    assert(p_parent != NULL);

	if (m_currentToken == NULL)
		return;

    ParseTreeNode* current = NULL;
    current = p_parent->AddChild(new ParseTreeNode("term"));
	Factor(current);

	while(m_currentToken && g_TokenTypesMap[m_currentToken->TypeId]->Name == "*")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		Factor(current);
	}
}

//////////////////////////////////////////////////////////////////////////
void Parser::Exp(ParseTreeNode* p_parent)
{
    assert(p_parent != NULL);

	if (m_currentToken == NULL)
		return;

    ParseTreeNode* current = NULL;
    current = p_parent->AddChild(new ParseTreeNode("exp"));

	SimpleExp(current);

	if (m_currentToken == NULL)
		return;

	if (g_TokenTypesMap[m_currentToken->TypeId]->Name == "<" ||
        g_TokenTypesMap[m_currentToken->TypeId]->Name == "=")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		SimpleExp(current);
	}
}
//////////////////////////////////////////////////////////////////////////
void Parser::Statement(ParseTreeNode* p_parent)
{
    assert(p_parent != NULL);

	if (m_currentToken == NULL)
		return;

    ParseTreeNode* current = NULL;
    current = p_parent->AddChild(new ParseTreeNode("statement"));

	if (g_TokenTypesMap[m_currentToken->TypeId]->Name == "if")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		Exp(current);
		Match("then", current);
		StmtSequence(current);

		if (g_TokenTypesMap[m_currentToken->TypeId]->Name == "else")
		{
			Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
			StmtSequence(current);
		}

		Match("end", current);
	}

	
	else if (g_TokenTypesMap[m_currentToken->TypeId]->Name == "repeat")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		StmtSequence(current);
		Match("until", current);
		Exp(current);
	}
	
	else if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "identifier")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		Match(":=", current);
		Exp(current);
	}

	else if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "read")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		Match("identifier", current);
	}

	else if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "write")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		Exp(current);
	}

	//else
	//{
	//	m_output += ">> Error in matching statement << \n";
	//	//Error
	//}

}
//////////////////////////////////////////////////////////////////////////
void Parser::Factor(ParseTreeNode* p_parent)
{
    assert(p_parent != NULL);

	if (m_currentToken == NULL)
		return;

    ParseTreeNode* current = NULL;

    current = p_parent->AddChild(new ParseTreeNode("factor"));

	if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "(")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
		Exp(current);
		Match(")", current);
	}

	else if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "number")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
	}

	else if(g_TokenTypesMap[m_currentToken->TypeId]->Name == "identifier")
	{
		Match(g_TokenTypesMap[m_currentToken->TypeId]->Name, current);
	}

	//else
	//{
	//	//Error
	//	m_output += ">> Error in matching factor << \n";
	//}
}
//////////////////////////////////////////////////////////////////////////
Parser::~Parser()
{
    if(m_parseTree != NULL)
        delete m_parseTree;
}
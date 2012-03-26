#include "LexicalAnalyzer.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include "LanguageReader.h"
#include "ErrorData.h"
#include "Toolbox.h"
#include "CompilerSettings.h"

LexicalAnalyzer::LexicalAnalyzer(string p_lexerDefinitionPath)
{
    Initialize(p_lexerDefinitionPath);
}
//////////////////////////////////////////////////////////////////////////
LexicalAnalyzer::LexicalAnalyzer()
{
    Initialize(g_CompilerSettings.LexerDefinitionPath());
}
//////////////////////////////////////////////////////////////////////////
void LexicalAnalyzer::Initialize(string p_lexerDefinitionPath)
{
    m_buffer        = NULL;
    m_dfa           = NULL;
    Language* lang  = NULL;

    lang = g_LanguageReader.Read(p_lexerDefinitionPath);
    assert(lang != NULL);

    m_dfa = lang->Dfa;
    m_reservedWordsToTokenTypeIdMap.clear();
    m_reservedWordsToTokenTypeIdMap.insert(lang->ReservedWordsToTokenTypeIdMap.begin(), lang->ReservedWordsToTokenTypeIdMap.end());
}
//////////////////////////////////////////////////////////////////////////
void LexicalAnalyzer::Retract()
{
    m_forward--;
}

//////////////////////////////////////////////////////////////////////////
void LexicalAnalyzer::Reset()
{
    if(m_buffer != NULL && m_buffer->InnerBuffer().size() > 0)
    {
        m_lexemeBegin   = -1;
        m_forward       = -1;
        m_eof           = false;
        m_symbolTable.clear();
    }
    else
    {
        m_eof = true;
    }

    Toolbox::MemoryClean(m_recognizedTokens);
}

//////////////////////////////////////////////////////////////////////////
vector<Token*>* LexicalAnalyzer::GetSymbolTable()
{
    return &m_symbolTable;
}

//////////////////////////////////////////////////////////////////////////
void LexicalAnalyzer::SetCodeBuffer(CharacterBuffer* p_codeBuffer)
{
    if(m_buffer != NULL && p_codeBuffer != m_buffer)
        delete m_buffer;

    m_buffer = p_codeBuffer;
}


//////////////////////////////////////////////////////////////////////////
bool LexicalAnalyzer::IsEndOfFile()
{
    return m_eof;
}

//////////////////////////////////////////////////////////////////////////
bool LexicalAnalyzer::IsIgnoredToken(int p_tokenTypeId)
{
    return g_TokenTypesMap[p_tokenTypeId]->IsIgnored;
}

//////////////////////////////////////////////////////////////////////////
bool LexicalAnalyzer::IsSpecialToken( int p_tokenTypeId )
{
    return g_TokenTypesMap[p_tokenTypeId]->IsSpecial;
}

//////////////////////////////////////////////////////////////////////////
bool LexicalAnalyzer::IsCompositeToken(int p_tokenTypeId)
{
    return g_TokenTypesMap[p_tokenTypeId]->IsComposite;
}

//////////////////////////////////////////////////////////////////////////
bool LexicalAnalyzer::IsIdentifierToken(int p_tokenTypeId)
{
    return g_TokenTypesMap[p_tokenTypeId]->IsIdentifier;
}

//////////////////////////////////////////////////////////////////////////
bool LexicalAnalyzer::IsErrorToken(int p_tokenTypeId)
{
    return g_TokenTypesMap[p_tokenTypeId]->IsError;
}

//////////////////////////////////////////////////////////////////////////
void LexicalAnalyzer::OnFail(Token* p_errorToken)
{
    ErrorData* errorData;

    errorData = new ErrorData(ERROR_InvalidToken, p_errorToken->Value->Lexeme, p_errorToken->Value->Row, p_errorToken->Value->Column);

    Fail(this, errorData);
}

//////////////////////////////////////////////////////////////////////////
char LexicalAnalyzer::NextChar()
{
    m_forward++;
    if(m_forward >= (int)m_buffer->InnerBuffer().size())
    {
        m_eof = true;
        return EOF;
    }

    return m_buffer->operator [](m_forward);
}

//////////////////////////////////////////////////////////////////////////
Token* LexicalAnalyzer::EndOfFileToken()
{
    if(!m_buffer->InnerBuffer().empty())
    {
        PlainCharacter preEof = *m_buffer->InnerBuffer().back();
        return new Token(EOFTypeId,new TokenValue("", preEof.Row, preEof.Column));
    }
    else
    {
        return new Token(EOFTypeId,new TokenValue("", 0, 0));
    }
}

//////////////////////////////////////////////////////////////////////////
Token* LexicalAnalyzer::GetNextToken()
{
    if(IsEndOfFile())
        return EndOfFileToken();

    char c = NextChar();
    m_lexemeBegin = m_forward;
    m_dfa->Reset();

    int m_currentTokenTypeId = TOKEN_Unrecognized;
    while(true)
    {
        m_currentTokenTypeId = m_dfa->Move(c);
        if(m_currentTokenTypeId == TOKEN_Unrecognized)
        {
            c = NextChar();
        }
        else
        {
            break;
        }
    }

    Retract();

    if(IsIgnoredToken(m_currentTokenTypeId))
    {
        return GetNextToken();
    }
    else
    {
        Token*                      token;
        string                      lexeme;
        TokenValue*                 tokenValue;
        vector<PlainCharacter*>&    buffer = m_buffer->InnerBuffer();

        m_buffer->ExtractString(m_lexemeBegin, m_forward - m_lexemeBegin + 1, lexeme);

        tokenValue = new TokenValue("", buffer[m_lexemeBegin]->Row, buffer[m_lexemeBegin]->Column);
        token = new Token(m_currentTokenTypeId, tokenValue);

        if(IsCompositeToken(m_currentTokenTypeId))
        {
            if(m_reservedWordsToTokenTypeIdMap.find(lexeme) != m_reservedWordsToTokenTypeIdMap.end())
            {
                token->TypeId = g_TokenTypesMap[m_reservedWordsToTokenTypeIdMap[lexeme]]->Id;
                m_recognizedTokens.push_back(token);
                return token;
            }
        }

        if(IsSpecialToken(m_currentTokenTypeId))
        {
            m_recognizedTokens.push_back(token);
            return token;
        }
        else if(IsIdentifierToken(m_currentTokenTypeId))
        {
            // save the index of the identifier in the symbol table
            token->Value->Data = m_symbolTable.size();
            m_symbolTable.push_back(token);
        }

        token->Value->Lexeme = lexeme;

        if(IsErrorToken(m_currentTokenTypeId))
        {
            OnFail(token);
        }

        m_recognizedTokens.push_back(token);
        return token;
    }
}

//////////////////////////////////////////////////////////////////////////
LexicalAnalyzer::~LexicalAnalyzer()
{
    Toolbox::MemoryClean(m_recognizedTokens);
    Toolbox::MemoryClean(m_dfa);
    Toolbox::MemoryClean(m_buffer);
}

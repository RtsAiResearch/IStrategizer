#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include "DFA.h"
#include "CharacterBuffer.h"
#include <set>
#include "Event.h"

class LexicalAnalyzer;

typedef Event<LexicalAnalyzer> LexicalAnalyzerEvent;

class LexicalAnalyzer
{
    int                 m_lexemeBegin;
    int                 m_forward;
    CharacterBuffer*    m_buffer;
    bool                m_eof;
    DFA*                m_dfa;
    map<string, int>    m_reservedWordsToTokenTypeIdMap;
    vector<Token*>      m_symbolTable;
    vector<Token*>      m_recognizedTokens;
    void Retract();
    char NextChar();
    bool IsEndOfFile();

protected:
    virtual void OnFail(Token* p_errorToken);

public:
    LexicalAnalyzerEvent Fail;
    void SetCodeBuffer(CharacterBuffer* p_codeBuffer);
    void Reset();
    void Initialize( string p_lexerDefinitionPath);
    bool IsIgnoredToken(int p_tokenTypeId);
    bool IsSpecialToken(int p_tokenTypeId);
    bool IsCompositeToken(int p_tokenTypeId);
    bool IsIdentifierToken(int p_tokenTypeId);
    bool IsErrorToken(int p_tokenTypeId);
    Token* GetNextToken();
    const vector<Token*>* RecognizedTokens() const { return &m_recognizedTokens; }
    vector<Token*>* GetSymbolTable();
    
    LexicalAnalyzer();
    LexicalAnalyzer( string p_lexerDefinitionPath);
    ~LexicalAnalyzer();
    Token* EndOfFileToken();
};

#endif // LEXICALANALYZER_H

#ifndef UNREALCOMPILER_H
#define UNREALCOMPILER_H

#include "LexicalAnalyzer.h"
#include "Parser.h"
#include "CharacterBuffer.h"
#include "Event.h"
#include "ErrorData.h"
#include "AbstractParser.h"
#include "AbstractTranslator.h"

class UnrealCompiler;

typedef Event<UnrealCompiler> CompilerEvent;
typedef Delegate<UnrealCompiler, LexicalAnalyzer> LexicalAnalyzerToCompilerDelegate;
typedef Delegate<UnrealCompiler, AbstractParser> AbstractParserToCompilerDelegate;
typedef Delegate<UnrealCompiler, AbstractTranslator> AbstractTranslatorToCompilerDelegate;

class UnrealCompiler
{
private:
    LexicalAnalyzer*    m_lexicalAnalyzer;
	AbstractParser*	    m_parser;
    AbstractTranslator* m_translator;
    vector<ErrorData*>  m_errors;

    void ScannerFailHandler(const LexicalAnalyzer* p_sender, void* p_parameter);
    void ParserFailHandler(const AbstractParser* p_sender, void* p_parameter);
    void TranslatorFailHandler(const AbstractTranslator* p_sender, void* p_parameter);
    //////////////////////////////////////////////////////////////////////////
protected:
    virtual void OnCompilationError(ErrorData* p_errorData);
    virtual void OnCompilationEnded();
    //////////////////////////////////////////////////////////////////////////
public:
    CompilerEvent CompilationError;
    CompilerEvent CompilationEnded;
    void                        Reset();
    void                        Compile(CharacterBuffer* p_codeBuffer);
    const ParseTreeNode*        ParseTree() const { return m_parser->ParseTree(); }
    const vector<Token*>*       RecognizedTokens() const { return m_lexicalAnalyzer->RecognizedTokens(); }
    const vector<ErrorData*>*   Errors() const { return &m_errors; }

    UnrealCompiler(void);
    ~UnrealCompiler(void);
};

#endif // UNREALCOMPILER_H

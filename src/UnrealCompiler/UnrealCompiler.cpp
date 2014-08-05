#include "UnrealCompiler.h"
#include "PredictiveParser.h"
#include "SyntaxDirectedTranslator.h"
#include "Toolbox.h"

//////////////////////////////////////////////////////////////////////////
UnrealCompiler::UnrealCompiler()
{
    m_lexicalAnalyzer = new LexicalAnalyzer();
    m_lexicalAnalyzer->Fail += new LexicalAnalyzerToCompilerDelegate(this, &UnrealCompiler::ScannerFailHandler);

    m_parser = new PredictiveParser(m_lexicalAnalyzer);
    m_parser->Fail += new AbstractParserToCompilerDelegate(this, &UnrealCompiler::ParserFailHandler);

    m_translator = new SyntaxDirectedTranslator(m_parser->Grammar());
    m_translator->Fail += new AbstractTranslatorToCompilerDelegate(this, &UnrealCompiler::TranslatorFailHandler);
}

//////////////////////////////////////////////////////////////////////////
void UnrealCompiler::OnCompilationError(ErrorData* p_errorData)
{
    CompilationError(this, p_errorData);
}

//////////////////////////////////////////////////////////////////////////
void UnrealCompiler::OnCompilationEnded()
{
    CompilationEnded(this, NULL);
}

//////////////////////////////////////////////////////////////////////////
void UnrealCompiler::ScannerFailHandler(const LexicalAnalyzer* p_sender, void* p_parameter)
{
    ErrorData* m_errorData = NULL;

    m_errorData = static_cast<ErrorData*>(p_parameter);
    assert(m_errorData != NULL);

    m_errors.push_back(m_errorData);

    OnCompilationError(m_errorData);
}

//////////////////////////////////////////////////////////////////////////
void UnrealCompiler::ParserFailHandler(const AbstractParser* p_sender, void* p_parameter)
{
    ErrorData* m_errorData = NULL;

    m_errorData = static_cast<ErrorData*>(p_parameter);
    assert(m_errorData != NULL);

    m_errors.push_back(m_errorData);

    OnCompilationError(m_errorData);
}
//////////////////////////////////////////////////////////////////////////
void UnrealCompiler::TranslatorFailHandler(const AbstractTranslator* p_sender, void* p_parameter)
{
    ErrorData* m_errorData = NULL;

    OnCompilationError(m_errorData);
}

//////////////////////////////////////////////////////////////////////////
void UnrealCompiler::Reset()
{
    m_lexicalAnalyzer->Reset();
    m_parser->Reset();
    m_errors.clear();
}

//////////////////////////////////////////////////////////////////////////
void UnrealCompiler::Compile(CharacterBuffer* p_codeBuffer)
{
    m_lexicalAnalyzer->SetCodeBuffer(p_codeBuffer);

    Reset();
    m_parser->Parse();

    if(m_errors.empty())
        m_translator->Translate(m_parser->ParseTree());

    OnCompilationEnded();
}

//////////////////////////////////////////////////////////////////////////
UnrealCompiler::~UnrealCompiler(void)
{
    Toolbox::MemoryClean(m_errors);
    Toolbox::MemoryClean(m_lexicalAnalyzer);
    Toolbox::MemoryClean(m_parser);
    Toolbox::MemoryClean(m_translator);
}


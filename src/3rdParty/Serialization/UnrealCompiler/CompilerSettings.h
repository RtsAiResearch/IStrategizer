#ifndef COMPILERSETTINGS_H
#define COMPILERSETTINGS_H

#include <string>
using namespace std;

class CompilerSettings
{
public:
    static CompilerSettings& Instance() { static CompilerSettings instance; return instance; }
    string GrammarDefinitionPath() { return "Tiny(C)SDT.txt"; }
    string LexerDefinitionPath() { return "Tiny(C)Tokens.txt"; }
    
    #define g_CompilerSettings CompilerSettings::Instance() 
};

#endif // COMPILERSETTINGS_H

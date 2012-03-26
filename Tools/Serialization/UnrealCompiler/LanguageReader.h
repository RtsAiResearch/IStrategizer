#ifndef LanguageReader_H
#define LanguageReader_H

#include <string>
#include <map>
#include <fstream>

#include "DFA.h"

using namespace std;

struct Language
{
    DFA*                Dfa;
    map<string, int>    ReservedWordsToTokenTypeIdMap;
    Language(DFA* p_dfa, const map<string, int>& p_reservedWordsToTokenTypeIdMap) : Dfa(p_dfa), ReservedWordsToTokenTypeIdMap(p_reservedWordsToTokenTypeIdMap) 
    {}
};

class LanguageReader
{
    
    int                 m_startState;
    int                 m_errorState;
    vector<DFAState*>   m_states;
    map<string, string> m_variableTable;
    map<string, char>   m_specialCharTable;
    map<string, int>    m_tokenTypeTable;
    map<string, int>    m_reservedWordsToTokenTypeIdMap;

    char GetSpecialCharacter(const string& p_specialChar);
    void ReadVariables(ifstream& p_eye);
    void ReadTokenTypes(ifstream& p_eye);
    void ReadStates(ifstream& p_eye);
    void ReadTags(ifstream& p_eye);
    LanguageReader();
public:
    static LanguageReader& Instance();
    Language* Read(string p_definitionFilePath);
    void ReadReservedWords( ifstream& p_eye );
};



#define g_LanguageReader LanguageReader::Instance()

#endif // LanguageReader_H
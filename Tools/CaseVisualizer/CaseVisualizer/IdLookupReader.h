#ifndef IDLOOKUPREADER_H
#define IDLOOKUPREADER_H

#include <hash_map>
#include <string>
using namespace std;
using namespace stdext;

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif
using namespace DataStructure;

#define LexerFile "TypeInfo\\TypeLexer.dfa"

class LexicalAnalyzer;
class CharacterBuffer;

class IdLookupReader
{
    LexicalAnalyzer* m_scanner;
    CharacterBuffer* m_buffer;

    string FilterLine(const string& p_line);
    string EnumToSentence(const string& p_enum);

public:
    IdLookupReader();
    ~IdLookupReader();
    void Read(const char* p_filePath, CrossMap<unsigned, string>& p_table);
	bool ReadEx(const char* p_filePath, CrossMap<unsigned, string>& p_table);
};

#endif // IDLOOKUPREADER_H
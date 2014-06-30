#ifndef IDLOOKUPREADER_H
#define IDLOOKUPREADER_H

#include <hash_map>
#include <string>

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

#define LexerFile "TypeInfo\\TypeLexer.dfa"

class LexicalAnalyzer;
class CharacterBuffer;

namespace IStrategizer
{
    class IdLookupReader
    {
        LexicalAnalyzer* m_scanner;
        CharacterBuffer* m_buffer;

        std::string FilterLine(const std::string& p_line);
        std::string EnumToSentence(const std::string& p_enum);

    public:
        IdLookupReader();
        ~IdLookupReader();
        void Read(const char* p_filePath, CrossMap<unsigned, std::string>& p_table);
        bool ReadEx(const char* p_filePath, CrossMap<unsigned, std::string>& p_table);
    };
}

#endif // IDLOOKUPREADER_H

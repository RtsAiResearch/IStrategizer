#ifndef IDLOOKUPREADER_H
#include "IdLookupReader.h"
#endif
#ifndef CHARACTERBUFFER_H
#include "CharacterBuffer.h"
#endif
#ifndef LEXICALANALYZER_H
#include "LexicalAnalyzer.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif

#include <sstream>
#include <fstream>
#include <cassert>
#include <string>
using namespace std;

IdLookupReader::IdLookupReader()
{
    m_scanner   = new LexicalAnalyzer(LexerFile);
    m_buffer    = new CharacterBuffer;
}
//----------------------------------------------------------------------------------------------
IdLookupReader::~IdLookupReader()
{
    Toolbox::MemoryClean(m_scanner);
}
//----------------------------------------------------------------------------------------------
void IdLookupReader::Read(const char* p_filePath, CrossMap<unsigned, string>& p_table)
{
    fstream file;
    file.open(p_filePath, ios::in);
    
	if (!file.is_open())
		return;

    string      line;
    string      key;
    string      value;
    Token*      token = NULL;
    unsigned    lastId = 0;

    while(getline(file, line))
    {
        if(line.empty())
            continue;

        line = FilterLine(line);
        Toolbox::GetCharacterBuffer(line, m_buffer);
        m_scanner->SetCodeBuffer(m_buffer);
        m_scanner->Reset();

        key.clear();
        value.clear();

        while((token = m_scanner->GetNextToken()) && !token->IsEOF())
        {
            if (g_TokenTypesMap[token->TypeId]->Name == "identifier")
            {
                value = token->Value->Lexeme;
            }
            else if(g_TokenTypesMap[token->TypeId]->Name == "number")
            {
                key = token->Value->Lexeme;
                lastId = atoi(key.c_str());
            }
        }

        if(key.empty())
            ++lastId;

        p_table.SetByFirst(lastId, EnumToSentence(value));
    }

    file.close();
}
//----------------------------------------------------------------------------------------------
bool IdLookupReader::ReadEx(const char* p_filePath, CrossMap<unsigned, string>& p_table)
{
	fstream file;
	file.open(p_filePath, ios::in);
	
	if (!file.is_open())
		return false;

	string			line;
	string			key;
	string			value;
	string			temp;
	unsigned		lastId = 0;

	while(getline(file, line))
	{
		if(line.empty())
			continue;
		
		stringstream stream;

		stream.str(line);

		stream >> key ;
		lastId = atoi(key.c_str());

		value.clear();
		while (!stream.eof())
		{
			stream >> temp;
			value += temp;

			if (!stream.eof() &&
				stream.peek() != '\n' &&
				stream.peek() != '\r')
				value += ' ';
		}
			
		p_table.SetByFirst(lastId, value);
	}

	file.close();

	return true;
}
//----------------------------------------------------------------------------------------------
string IdLookupReader::FilterLine(const string& p_line)
{
    string newLine;
    newLine.reserve(p_line.size());

    for(int i = 0, size = p_line.size(); i < size; ++i)
    {
        if(p_line[i] > 0)
            newLine += p_line[i];
    }

    return newLine;
}
//----------------------------------------------------------------------------------------------
string IdLookupReader::EnumToSentence(const string& p_enum)
{
    int where = p_enum.find('_');
    return p_enum.substr(where + 1, p_enum.size() - where);

    /*string      sentence;
    int         current = 0;
    int         forward = 0;
    int         state = 0;
    const int   error = 4;

    while(forward < p_enum.size() && state != error)
    {
        switch(state)
        {
        case 0:
            if(p_enum[forward] == '_')
                state = 1;
            else if(islower(p_enum[forward]))
                state = error;
            break;
        case 1:
            if(isupper(p_enum[forward]))
            {
                state = 2;
                current = forward;
            }
            else
                state = error;
            break;
        case 2:
            if(islower(p_enum[forward]) || isdigit(p_enum[forward]))
                state = 3;
            else
                state = error;
            break;
        case 3:
            if(isupper(p_enum[forward]) && forward + 1 < p_enum.size())
            {
                sentence += p_enum.substr(current, forward - current);
                sentence += ' ';
                state = 2;
                current = forward;
            }
            else if(forward + 1 == p_enum.size())
            {
                sentence += p_enum.substr(current, forward - current + 1);
            }
        }

        ++forward;
    }

    assert(state != error);*/

    //return sentence;
}
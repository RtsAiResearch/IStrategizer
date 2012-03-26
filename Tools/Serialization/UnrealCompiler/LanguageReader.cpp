#include "LanguageReader.h"
#include <sstream>
#include <vector>
#include "AlphabetFactory.h"
#include "DFAState.h"
#include <cassert>

vector<TokenType*> g_TokenTypesMap(MaxTokenTypes, NULL);
int EOFTypeId;

//////////////////////////////////////////////////////////////////////////
LanguageReader& LanguageReader::Instance()
{
    static LanguageReader instance;
    return instance;
}

//////////////////////////////////////////////////////////////////////////
LanguageReader::LanguageReader()
{
    m_specialCharTable["\\b"] = -1;
    m_specialCharTable["\\t"] = '\t';
    m_specialCharTable["\\s"] = ' ';
    m_specialCharTable["\\r"] = '\r';
    m_specialCharTable["\\n"] = '\n';
}

//////////////////////////////////////////////////////////////////////////
char LanguageReader::GetSpecialCharacter( const string& p_specialChar )
{
    if(m_specialCharTable.find(p_specialChar) == m_specialCharTable.end())
    {
        return p_specialChar[0];
    }

    return m_specialCharTable[p_specialChar];
}



//////////////////////////////////////////////////////////////////////////
void LanguageReader::ReadVariables(ifstream& p_eye)
{
    string          line;
    string          identifier;
    string          elementCollection;
    string          element;
    stringstream    stream;

    m_variableTable.clear();
    while(getline(p_eye, line))
    {
        if(line == "")
            continue;

        stream.clear();
        stream.str(line);
        stream >> identifier;

        if(identifier == "#")
            break;

        elementCollection.clear();
        while(stream >> element)
        {
            elementCollection += GetSpecialCharacter(element);
        }

        string& var = m_variableTable[identifier];
        g_AlphabetFactory.GetAlphabet(var, elementCollection);
    }
}

//////////////////////////////////////////////////////////////////////////
void LanguageReader::ReadTokenTypes(ifstream& p_eye )
{
    char            tokenTypeTag;
    int             tokenTypeId;
    string          line;
    string          tokenTypeName;
    stringstream    stream;
    TokenType*      tokenType;

    m_reservedWordsToTokenTypeIdMap.clear();
    while(getline(p_eye, line))
    {
        if(line == "")
            continue;

        stream.clear();
        stream.str(line);

        stream >> tokenTypeName;

        if(tokenTypeName == "#")
            break;

        stream >> tokenTypeId;
        
        tokenType = new TokenType(tokenTypeId, tokenTypeName);

        g_TokenTypesMap[tokenTypeId] = tokenType;
        m_tokenTypeTable[tokenTypeName] = tokenTypeId;

        while(stream >> tokenTypeTag)
        {
            switch(tokenTypeTag)
            {
            case 'e':
                tokenType->IsError = true;
                break;
            case 'c':
                tokenType->IsComposite = true;
                break;
            case 'i':
                tokenType->IsIdentifier = true;
                break;
            case 's':
                tokenType->IsSpecial = true;
                break;
            case 'g':
                tokenType->IsIgnored = true;
                break;
            case 'r':
                tokenType->IsReserved = true;
                m_reservedWordsToTokenTypeIdMap.insert(make_pair(tokenTypeName, tokenTypeId));
                break;
            }
        }
    }

    EOFTypeId = tokenTypeId + 1;
    g_TokenTypesMap[EOFTypeId] = new TokenType(EOFTypeId, "EOF");

}


//////////////////////////////////////////////////////////////////////////
void LanguageReader::ReadStates(ifstream& p_eye)
{
    int             curentStateId;
    int             targetStateId;
    int             numberOfTransitions;
    string          tokenTypeName;
    string          line;
    string          input;
    stringstream    stream;
    vector<pair<string, int> >  transitionFunction;
    
    m_states.clear();
    while(getline(p_eye, line) && line != "#")
    {
        if(line == "")
            continue;

        stream.clear();
        stream.str(line);

        stream >> curentStateId;
        stream >> tokenTypeName;
        stream >> numberOfTransitions;
        
        transitionFunction.clear();
        for(int transitionIdx = 0; transitionIdx < numberOfTransitions; transitionIdx++)
        {
            getline(p_eye, line);
            stream.clear();
            stream.str(line);

            stream >> input;
            stream >> targetStateId;

            if(m_variableTable.find(input) != m_variableTable.end())
                transitionFunction.push_back(make_pair(m_variableTable[input], targetStateId));
            else
                transitionFunction.push_back(make_pair(input, targetStateId));

        }

        m_states.push_back(new DFAState(curentStateId, m_tokenTypeTable[tokenTypeName], transitionFunction));
    }
}

//////////////////////////////////////////////////////////////////////////
void LanguageReader::ReadTags(ifstream& p_eye)
{
    int             tagValue;
    string          line;
    string          tageName;
    stringstream    stream;

    while(getline(p_eye, line) && line != "#")
    {
        if(line == "")
            continue;

        stream.clear();
        stream.str(line);

        stream >> tageName;
        stream >> tagValue;
        
        if(tageName == "start")
        {
            m_startState = tagValue;
        }
        else if(tageName == "error")
        {
            m_errorState = tagValue;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
Language* LanguageReader::Read( string p_definitionFilePath )
{
    ifstream eye;
    eye.open(p_definitionFilePath.c_str());

    if(eye.is_open())
    {
        ReadVariables(eye);
        ReadTokenTypes(eye);
        ReadStates(eye);
        ReadTags(eye);

        eye.close();

        return new Language(new DFA(m_startState, m_errorState, m_states), m_reservedWordsToTokenTypeIdMap);
    }

    return NULL;
}


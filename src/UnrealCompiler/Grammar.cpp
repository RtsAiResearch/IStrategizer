#include "Grammar.h"
#include <fstream>
#include <sstream>
#include <cassert>

string Epsilon = "";

//////////////////////////////////////////////////////////////////////////
void Grammar::Initialize(const string& p_definitionPath)
{
    ifstream eye;
    eye.open(p_definitionPath.c_str());

    assert(eye.is_open());

    ReadNonTerminals(eye);
    ReadTerminals(eye);
    ReadProductions(eye);
    ReadTags(eye);

    eye.close();
}
//////////////////////////////////////////////////////////////////////////
void Grammar::ReadNonTerminals(ifstream& p_eye)
{
    string          line;
    string          symbol;
    stringstream    stream;

    while(getline(p_eye, line))
    {
        if(line == "")
            continue;

        stream.clear();
        stream.str(line);
        stream >> symbol;

        if(!stream || symbol == "#")
            break;
        else
            NonTerminals.insert(symbol);
    }
}

//////////////////////////////////////////////////////////////////////////
void Grammar::ReadTerminals(ifstream& p_eye)
{
    string          line;
    string          symbol;
    stringstream    stream;

    while(getline(p_eye, line))
    {
        if(line == "")
            continue;

        stream.clear();
        stream.str(line);
        stream >> symbol;

        if(!stream || symbol == "#")
            break;
        else
            Terminals.insert(symbol);
    }
}

//////////////////////////////////////////////////////////////////////////
void Grammar::ReadProductions( ifstream& eye )
{
    string                      line;
    stringstream                lineStream;
    ProductionHead              nonTerminal;
    string                      symbol;

    while(getline(eye, line))
    {
        if(line.empty())
            continue;

        lineStream.clear();
        lineStream.str(line);

        lineStream >> nonTerminal;

        if(!lineStream || nonTerminal == "#")
            break;

        ProductionBody production;
        while(lineStream >> symbol)
        {
            production.Production.push_back(symbol);
        }
            
        assert(!production.Production.empty());
        Productions[nonTerminal].push_back(production);
    }
}
//////////////////////////////////////////////////////////////////////////
void Grammar::ReadTags( ifstream& p_eye )
{
    string          line;
    string          symbol;
    string          option;
    stringstream    stream;

    while(getline(p_eye, line))
    {
        if(line == "")
            continue;

        stream.clear();
        stream.str(line);

        stream >> option;
        stream >> symbol;

        if(option == "start")
            Start = symbol;
        else if(option == "epsilon")
        {
            Terminals.insert(symbol);
            Epsilon = symbol;
        }
    }
}

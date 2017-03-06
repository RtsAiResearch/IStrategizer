#include "AlphabetFactory.h"
#include <sstream>
using namespace std;

void AlphapetFactory::GetAlphabet(string& p_outSet, const string& p_inSet)
{
    stringstream alphabetStream;
    char start, end;

    if(p_inSet.length() < 1)
    {
        p_outSet = p_inSet;
        return;
    }

    // range
    // pattern  a-z
    // index    012
    if (p_inSet.length() == 3 &&
        p_inSet[1] == '-')
    {
        start   = p_inSet[0];
        end     = p_inSet[2];

        if(start > end)
        {
            swap(start, end);
        }

        for(int c = start; c <= end; c++)
        {
            alphabetStream << char(c);
        }
    }

    // complement range
    // pattern  ^a-z
    // index    0123
    else if (p_inSet.length() == 4 &&
        p_inSet[0] == '^' &&
        p_inSet[2] == '-')
    {
        start   = p_inSet[1];
        end     = p_inSet[3];

        if(start > end)
        {
            swap(start, end);
        }

        for(int c = 1; c <= 255; c++)
        {
            if(c > end || c < start)
                alphabetStream << char(c);
        }
    }

    // complement set
    // pattern  ^qwert...
    // index    012345..n
    else if (p_inSet.length() > 1 &&
        p_inSet[0] == '^')
    {
        memset(m_alphabet, 0, sizeof(char) * 255);
        for(unsigned c = 1; c < p_inSet.length(); c++)
        {
            m_alphabet[p_inSet[c]] = true;
        }

        for(unsigned c = 1; c <= 255; c++)
        {
            if(!m_alphabet[c])
            {
                alphabetStream << char(c);
            }
        }
    }

    else if(p_inSet == ".")
    {
        for(int c = 1; c <= 255; c++)
        {
            alphabetStream << char(c);
        }
    }

    // set
    // pattern  [qwert...]
    // index    012345...n
    else
    {
        for(unsigned c = 0; c < p_inSet.length(); c++)
        {
            alphabetStream << char(p_inSet[c]);
        }
    }

    p_outSet = alphabetStream.str();
}

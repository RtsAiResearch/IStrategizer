#ifndef ALPHABETFACTORY_H
#define ALPHABETFACTORY_H

#include <string>
using namespace std;

class AlphapetFactory
{
    bool m_alphabet[255];
protected:
    AlphapetFactory();
public:
    static AlphapetFactory& Instance();
    void GetAlphabet(string& p_outSet, const string& p_inSet);
};

//////////////////////////////////////////////////////////////////////////
inline AlphapetFactory::AlphapetFactory()
{

}

//////////////////////////////////////////////////////////////////////////
inline AlphapetFactory& AlphapetFactory::Instance()
{
    static AlphapetFactory instance;
    return instance;
}

#define g_AlphabetFactory AlphapetFactory::Instance()

#endif // ALPHABETFACTORY_H
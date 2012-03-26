#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include "ProductionBody.h"
using namespace std;

extern string Epsilon;


class Grammar
{
public:
    set<string> Terminals;
    set<string> NonTerminals;
    string      Start;
    map<ProductionHead, vector<ProductionBody> > Productions;
    virtual void Initialize(const string& p_definitionPath);
protected:
    virtual void ReadTerminals(ifstream& p_eye);
    virtual void ReadNonTerminals(ifstream& p_eye);
    virtual void ReadProductions( ifstream& p_eye );
    virtual void ReadTags( ifstream& p_eye );
    //////////////////////////////////////////////////////////////////////////
};

#endif // GRAMMAR_H
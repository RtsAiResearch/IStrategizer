#ifndef TYPETABLE_H
#define TYPETABLE_H

#ifndef TYPENODE_H
	#include "TypeNode.h"
#endif

#include <string>
#include <vector>
#include <map>
using namespace std;

class TypeData
{
public:
    TypeNode*           TypeGraph;
    vector<TypeNode*>   Parents;
    bool                IsAlias;
    TypeData() : TypeGraph(NULL), IsAlias(false) {}

    void Write(string p_path);
    void Read(string p_path);
};

typedef map<string, TypeData> TypeTable;

#endif // TYPETABLE_H
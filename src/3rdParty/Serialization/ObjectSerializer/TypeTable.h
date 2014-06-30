#ifndef TYPETABLE_H
#define TYPETABLE_H

#include <string>
#include <vector>
#include <map>

class TypeNode;

class TypeData
{
public:
    TypeNode*           TypeGraph;
    std::vector<TypeNode*>   Parents;
    bool                IsAlias;
    TypeData() : TypeGraph(NULL), IsAlias(false) {}

    void Write(std::string p_path);
    void Read(std::string p_path);
};

typedef std::map<std::string, TypeData> TypeTable;

#endif // TYPETABLE_H

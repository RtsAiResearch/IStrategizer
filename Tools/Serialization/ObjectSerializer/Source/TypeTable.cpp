#ifndef TYPETABLE_H
#include "TypeTable.h"
#endif

#include <fstream>
#include <cassert>
using namespace std;

void TypeData::Read(string p_path)
{
    fstream eye;
    p_path += TypeExtension;

    eye.open(p_path.c_str(), ios::binary | ios::in);
    assert(eye.is_open());

    TypeGraph = TypeNode::Read(eye);

    int parentsCount = 0;
    eye.read(reinterpret_cast<char*>(&parentsCount), sizeof(int));
    for(int i = 0; i < parentsCount; ++i)
    {
        Parents.push_back(TypeNode::Read(eye));
    }

    eye.close();
}
//----------------------------------------------------------------------------------------------
void TypeData::Write(string p_path)
{
    fstream pen;
    p_path += TypeGraph->UserDefinedType;
    p_path += TypeExtension;

    pen.open(p_path.c_str(), ios::binary | ios::out);
    assert(pen.is_open());

    TypeNode::Write(TypeGraph, pen);

    int parentsCount = Parents.size();
    pen.write(reinterpret_cast<char*>(&parentsCount), sizeof(int));
    for(int i = 0; i < parentsCount; ++i)
    {
        TypeNode::Write(Parents[i], pen);
    }

    pen.close(); 
}
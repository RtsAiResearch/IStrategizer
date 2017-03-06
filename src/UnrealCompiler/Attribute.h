#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <map>
#include <string>
#include <cassert>
using namespace std;

enum AttributeName
{
    ATTRNAME_Null,
    ATTRNAME_SynVal,
    ATTRNAME_InhVal,
    ATTRNAME_LexVal
};

typedef int AttributeType;

struct Attribute
{
    AttributeName   Name;
    AttributeType   Value;
    bool            Evaluated;
    Attribute(AttributeName p_name) : Name(p_name), Value(AttributeType()), Evaluated(false) {}
};

struct AttributeSet
{
    int         ProductionRelativeIdx;
    Attribute   SynVal;
    Attribute   InhVal;
    Attribute   LexVal;

    AttributeSet() : ProductionRelativeIdx(-1), SynVal(ATTRNAME_SynVal), InhVal(ATTRNAME_InhVal), LexVal(ATTRNAME_LexVal) {}
    Attribute& operator[](AttributeName p_attrName)
    {
        switch(p_attrName)
        {
        case ATTRNAME_InhVal:
            return InhVal;
            break;

        case ATTRNAME_SynVal:
            return SynVal;
            break;

        case ATTRNAME_LexVal:
            return LexVal;
            break;
            
        default:
            assert(false);
            return LexVal;
        }
    }
};

#endif // ATTRIBUTE_H

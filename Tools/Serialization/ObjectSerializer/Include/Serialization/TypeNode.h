#ifndef TYPENODE_H
#define TYPENODE_H

#include <fstream>
#include <queue>
using namespace std;

enum DataType
{
    DTYPE_Bool,
    DTYPE_Char,
    DTYPE_Short,
    DTYPE_Int,
    DTYPE_Float,
    DTYPE_Double,
    DYTPE_BasicTypeCount,
    DTYPE_Array,
    DTYPE_String,
    DTYPE_Map,
    DTYPE_Set,
    DTYPE_Vector,
    DTYPE_Pair,
    DTYPE_UserDefined,
    DTYPE_Template,
    DTYPE_Undefined
};

#define MaxTypeNameLength 127
#define TypeExtension ".type"

class TypeNode;
struct TypeChild
{
    union
    {
        unsigned    Val32;
        TypeNode*   Ptr32;
    };
    bool IsType;
    TypeChild() : Val32(0), IsType(false) {}
    TypeChild(unsigned p_val32) : Val32(p_val32), IsType(false) {}
    TypeChild(TypeNode* p_ptr32) : Ptr32(p_ptr32), IsType(true) {}
};

extern unsigned LastTypeNodeId;
extern unsigned TypeNodesCount;

class TypeNode
{
public:
    unsigned            Id;
    DataType            Type;
    bool                Indirection;
    string              UserDefinedType;
    vector<TypeChild>   Children;
    vector<TypeNode*>   TemplateArguments;

            TypeNode() : Type(DTYPE_Undefined), Indirection(false) { Id = ++LastTypeNodeId; ++TypeNodesCount; }
            TypeNode(DataType p_type) : Type(p_type), Indirection(false) { Id = ++LastTypeNodeId; ++TypeNodesCount; }
            TypeNode* Clone();
    void    SpecializeChildren(TypeNode* p_template, TypeNode* p_specialization);
    void    SpecializeTemplateArguments(TypeNode* p_template, TypeNode* p_specialization);
    void    SubstitueTypeName(const string& p_oldTypeName, const string& p_newTypeName);
    void    SetTemplateArguments(vector<TypeNode*>& p_templates);
    void    PromoteChildren(bool p_setAsTemplates = false);
    void    DisposeChildren();
    string  FullName();
            ~TypeNode();

protected:
    void    FullNameAux(string& p_str,  bool p_templateFlag = true, bool p_childrenFlag = true);
    string  ToString(DataType p_type);
//----------------------------------------------------------------------------------------------
public:
    static void         Write(TypeNode* p_root, fstream& p_pen);
    static TypeNode*    Read(fstream& p_eye);

protected:
    static void         WriteTemplateData(TypeNode* p_typeNode, fstream& p_pen);
    static void         ReadTemplateData(TypeNode* p_typeNode, fstream& p_eye);
};


#endif // TYPENODE_H
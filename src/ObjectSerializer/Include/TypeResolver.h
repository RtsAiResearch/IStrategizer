#ifndef TYPERESOLVER_H
#define TYPERESOLVER_H

#ifndef TYPETABLE_H
    #include "TypeTable.h"
#endif

#ifndef OBJECTFACTORY_H
    #include "ObjectFactory.h"
#endif
#include "TypeNode.h"

#include <set>
using namespace std;

class TypeResolver
{
    set<string> m_completeTypes;
    //----------------------------------------------------------------------------------------------
    void        TypeMemberSubstitution( TypeData& p_tableEntry, TypeTable& p_typeTable, vector<TypeChild>& p_children );
    void        TypeParentSubstitution( TypeData& p_tableEntry, TypeTable& p_typeTable, vector<TypeChild>& p_children );
    void        ShallowTypeSubstitution(const string& p_typeName, TypeTable& p_typeTable, vector<TypeChild>& p_children);
    TypeNode*   AliasSubstitution(const TypeNode* p_typeNode, const TypeTable& p_typeTable);
    void        SpecializeAux(TypeNode* p_targetType, TypeNode* p_specialization, TypeTable& p_typeTable);

public:
                TypeResolver() {}
    void        Resolve(TypeTable& p_typeTable);
    void        Specialize(TypeNode* p_specialization, TypeData& p_typeTemplate, TypeTable& p_typeTable);

    static TypeResolver& Instance() { static TypeResolver instance; return instance; }
#define     g_TypeResolver TypeResolver::Instance()
};

#endif // TYPERESOLVER_H

#ifndef TYPERESOLVER_H
#include "TypeResolver.h"
#endif

#ifndef OBJECTFACTORY_H
#include "ObjectFactory.h"
#endif

#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#include <Windows.h>
#include <cassert>

//----------------------------------------------------------------------------------------------
void TypeResolver::Resolve(TypeTable& p_typeTable)
{
    m_completeTypes.clear();
    vector<TypeChild>   newChildren;

    for(TypeTable::iterator typeItr = p_typeTable.begin();
        typeItr != p_typeTable.end();
        ++typeItr)
    {
        if(typeItr->second.IsAlias)
            continue;

        newChildren.clear();
        ShallowTypeSubstitution(typeItr->first, p_typeTable, newChildren);
    }
}
//----------------------------------------------------------------------------------------------
void TypeResolver::ShallowTypeSubstitution(const string& p_typeName, TypeTable& p_typeTable, vector<TypeChild>& p_children)
{
    TypeData&           tableEntry  = p_typeTable[p_typeName];
    TypeNode*           typeGraph   = tableEntry.TypeGraph;

    // Memoization: Already computed, return all children
    if(m_completeTypes.find(p_typeName) != m_completeTypes.end())
    {
        TypeNode* currentChild;
        for(int childIdx = 0, childSize = typeGraph->Children.size();
            childIdx < childSize;
            ++childIdx)
        {
            _ASSERTE(typeGraph->Children[childIdx].IsType);
            currentChild = typeGraph->Children[childIdx].Ptr32;
            p_children.push_back(TypeChild(currentChild->Clone()));
        }
    }
    else
    {
        vector<TypeNode*>&  parents     = tableEntry.Parents;

        // Inheritance substitution
        if(!parents.empty())
        {
            TypeParentSubstitution(tableEntry, p_typeTable, p_children);
        }

        // Member substitution
        TypeMemberSubstitution(tableEntry, p_typeTable, p_children);
        m_completeTypes.insert(p_typeName);
    }
}
//----------------------------------------------------------------------------------------------
void TypeResolver::TypeParentSubstitution( TypeData& p_tableEntry, TypeTable& p_typeTable, vector<TypeChild>& p_children )
{   
    TypeNode*           typeGraph   = p_tableEntry.TypeGraph;
    vector<TypeNode*>&  parents     = p_tableEntry.Parents;
    TypeNode*           resultant   = NULL;
    vector<TypeChild>   parentChildren;

    // substitute parents by going deep until reaching a non-alias node
    for(int i = 0, size = parents.size(); i < size; ++i)
    {
        _ASSERTE(parents[i]->Type == DTYPE_UserDefined);

        resultant = AliasSubstitution(parents[i], p_typeTable);
        parentChildren.clear();
        ShallowTypeSubstitution(resultant->UserDefinedType, p_typeTable, parentChildren);

        if(!resultant->TemplateArguments.empty())
        {
            for(int childIdx = 0, childSize = parentChildren.size();
                childIdx < childSize;
                ++childIdx)
            {
                _ASSERTE(parentChildren[childIdx].IsType);
                TypeNode* currentChild = parentChildren[childIdx].Ptr32;
                if(!currentChild->TemplateArguments.empty())
                {
                    currentChild->SpecializeTemplateArguments(p_typeTable[resultant->UserDefinedType].TypeGraph, resultant);
                }
            }
        }

        p_children.insert(p_children.end(), parentChildren.begin(), parentChildren.end());
    }
}
//----------------------------------------------------------------------------------------------
void TypeResolver::TypeMemberSubstitution( TypeData& p_tableEntry, TypeTable& p_typeTable, vector<TypeChild>& p_children )
{
    TypeNode*           typeGraph   = p_tableEntry.TypeGraph;
    TypeNode*           resultant   = NULL;
    vector<TypeChild>&  oldChildren = typeGraph->Children;

    for(size_t i = 0; i < oldChildren.size(); ++i)
    {
        if(!oldChildren[i].IsType)
            continue;

        resultant = AliasSubstitution(oldChildren[i].Ptr32, p_typeTable);

        Toolbox::MemoryClean(oldChildren[i].Ptr32);
        oldChildren[i].Ptr32 = resultant;

        p_children.push_back(oldChildren[i]);
    }

    oldChildren.clear();
    oldChildren.reserve(p_children.size());
    oldChildren.insert(oldChildren.begin(), p_children.begin(), p_children.end());
}
//----------------------------------------------------------------------------------------------
TypeNode* TypeResolver::AliasSubstitution(const TypeNode* pUnresolvedType, const TypeTable& typeTable)
{
    // Expand a typedef to its basic types removing all recursive typedefs
    // For example:
    // typedef PlanStepEx* NodeValue
    // typedef AdjListDigraph<NodeValue> OlcbpPlanDigraph
    //
    // We call PlanStepEx* as the 'aliased type' for NodeValue because
    // NodeValue is used in the code as an alias for PlanStepEx* pointer type
    // and we call NodeValue as the 'type alias'
    //
    // For Example:
    // Input: OlcbpPlanDigraph*
    // Output: AdjListDigraph<PlanStepEx*>*
    //

    // user defined types should always have entries in the type-table
    if (pUnresolvedType->Type == DTYPE_UserDefined &&
        typeTable.find(pUnresolvedType->UserDefinedType) == typeTable.end())
        DebugBreak();

    TypeNode* pResolvedAliasedType = nullptr;

    // If it is a typedef, then resolve it to its basic types
    // typedef PlanStepEx* NodeValue
    // 
    // Input: NodeValue
    // Output: PlanStepEx*
    //
    if (pUnresolvedType->Type == DTYPE_UserDefined &&
        typeTable.at(pUnresolvedType->UserDefinedType).IsAlias)
    {
        // The only template argument for NodeValue would be PlanStepEx* if
        // typedef PlanStepEx* NodeValue exist and and the unresolved node is NodeValue
        const TypeNode* pLinkedUnresolvedTypeAlias = typeTable.at(pUnresolvedType->UserDefinedType).TypeGraph;

        // Aliases has only 1 template argument which is its typedef
        if (pLinkedUnresolvedTypeAlias->TemplateArguments.size() != 1)
            DebugBreak();

        const TypeNode* pUnlikedUnresolvedAliasedType = pLinkedUnresolvedTypeAlias->TemplateArguments[0];
        bool unlinkedUnresolvedAliasedTypeIndirection = pUnlikedUnresolvedAliasedType->Indirection;
        //TypeNode* pLinkedUnresolvedAliasedType = nullptr;

        // If the aliasedType is a user-defined type, then retrieve its complete
        // type data and persist the indirection info on it
        if(pUnlikedUnresolvedAliasedType->Type == DTYPE_UserDefined)
        {
            // Get PlanStepEx complete type info as the unresolved aliased type from the type table
            TypeNode* pLinkedUnresolvedAliasedType = pUnlikedUnresolvedAliasedType->Clone();
            pLinkedUnresolvedAliasedType ->Indirection = false;

            // If the aliased type in the typedef was a pointer then update the indirection
            // in the retrieved complete type data
            // For example: PlanStepEx* in typedef PlanStepEx* NodeValue
            //
            //pUnresolvedAliasedType->Indirection = unresolvedAliasedTypeIndirection;
            //pLinkedUnresolvedAliasedType->DisposeChildren();
            pResolvedAliasedType = AliasSubstitution(pLinkedUnresolvedAliasedType , typeTable);

            delete pLinkedUnresolvedAliasedType ;

            // More than 1 level of indirection is not allowed
            // For example:
            // typedef PlanStepEx* NodeValue
            // typedef NodeValue* NodeValuePtr
            // Alias substitution for NodeValuePtr is prohibited and not supported
            // Because more than one level of indirection in not supported in serialization
            // and deserialiaztion

            _ASSERTE(pResolvedAliasedType->Indirection ^ unlinkedUnresolvedAliasedTypeIndirection ^ pUnresolvedType->Indirection);

            pResolvedAliasedType->Indirection |= unlinkedUnresolvedAliasedTypeIndirection;
            pResolvedAliasedType->Indirection |= pUnresolvedType->Indirection;
        }
        else
        {
            pResolvedAliasedType = pUnlikedUnresolvedAliasedType->Clone();
        }
    }
    else
    {
         pResolvedAliasedType = pUnresolvedType->Clone();
    }

    //..For each template argument of the resolved type
    //....Make alias substitution for that template argument
    //
    // typedef PlanStepEx* NodeValue
    //
    // Input: AdjListDigraph<NodeValue>
    // Output: AdjListDigraph<PlanStepEx*>
    //
    TypeNode* tempNode = NULL;
    vector<TypeNode*>& templateArgs = pResolvedAliasedType->TemplateArguments;

    for(int argIdx = 0, argSize = templateArgs.size();
        argIdx < argSize;
        ++argIdx)
    {
        tempNode = AliasSubstitution(templateArgs[argIdx], typeTable);

        Toolbox::MemoryClean(templateArgs[argIdx]);
        templateArgs[argIdx] = tempNode;
    }

    //..For each member in the resolved type
    //....Make alias substitution for that member type
    //
    // typedef unsigned NodeID
    // typedef PlanStepEx* NodeValue
    // typedef set<NodeID> NodeSerializedSet
    // typedef pair<NodeValue, NodeSerializedSet>
    //
    // Input: AdjListDigraph<PlanStepEx*> such that map<NodeID, NodeEntry> is a class member
    // Output: AdjListDigraph<PlanStepEx*> such that map<unsigned, pair<PlanStepEx*, set<unsigned> > is a class member
    //
    vector<TypeChild>& children = pResolvedAliasedType->Children;
    for(int childIdx = 0, childSize = children.size();
        childIdx < childSize;
        ++childIdx)
    {
        if(!children[childIdx].IsType)
            continue;

        tempNode = AliasSubstitution(children[childIdx].Ptr32, typeTable);

        Toolbox::MemoryClean(children[childIdx].Ptr32);
        children[childIdx].Ptr32 = tempNode;
    }

    return pResolvedAliasedType;
}
////----------------------------------------------------------------------------------------------
//TypeNode* TypeResolver::AliasSubstitution(TypeNode* p_typeNode, TypeTable& p_typeTable)
//{
//    if (p_typeNode->Type == DTYPE_UserDefined)
//    {
//        // Recursively substitute each alias with its type
//        _ASSERTE(p_typeTable.find(p_typeNode->UserDefinedType) != p_typeTable.end());
//        // type already substituted in the table, could be alias or a user type
//        if(m_completeTypes.find(p_typeNode->UserDefinedType) != m_completeTypes.end())
//        {
//            TypeNode* clone = p_typeTable[p_typeNode->UserDefinedType].TypeGraph->Clone();
//            _ASSERTE(!(clone->Indirection && p_typeNode->Indirection));
//            clone->Indirection |= p_typeNode->Indirection;
//
//            /*if(!clone->TemplateArguments.empty())
//            clone->SpecializeTemplateArguments(clone, p_typeNode);*/
//
//            p_typeNode = clone;
//        }
//        else
//        {
//            TypeData&   tableEntry  = p_typeTable[p_typeNode->UserDefinedType];
//            bool        isAlias     = tableEntry.IsAlias;
//
//            if(isAlias)
//            {
//                _ASSERTE(tableEntry.TypeGraph->TemplateArguments.size() == 1);
//
//                TypeNode* aliasChild    = tableEntry.TypeGraph->TemplateArguments[0];
//                TypeNode* resultant     = NULL;
//
//                if(aliasChild->Type == DTYPE_UserDefined)
//                    aliasChild = p_typeTable[aliasChild->UserDefinedType].TypeGraph;
//
//                resultant = AliasSubstitution(aliasChild, p_typeTable);
//                // more than 1 level of indirection is not allowed
//                _ASSERTE(!(resultant->Indirection && p_typeNode->Indirection));
//
//                m_completeTypes.insert(tableEntry.TypeGraph->UserDefinedType);
//
//                Toolbox::MemoryClean(tableEntry.TypeGraph);
//                tableEntry.TypeGraph = p_typeNode = resultant;
//
//                p_typeNode->Indirection |= aliasChild->Indirection;
//                p_typeNode->DisposeChildren();
//            }
//        }
//    }
//
//    TypeNode*           resultant   = NULL;
//    vector<TypeNode*>&  templateArgs = p_typeNode->TemplateArguments;
//    for(int argIdx = 0, argSize = templateArgs.size();
//        argIdx < argSize;
//        ++argIdx)
//    {
//        resultant = AliasSubstitution(templateArgs[argIdx], p_typeTable);
//        resultant->DisposeChildren();
//
//        Toolbox::MemoryClean(templateArgs[argIdx]);
//        templateArgs[argIdx] = resultant;
//    }
//
//    vector<TypeChild>&  children    = p_typeNode->Children;
//    for(int childIdx = 0, childSize = children.size();
//        childIdx < childSize;
//        ++childIdx)
//    {
//        if(!children[childIdx].IsType)
//            continue;
//
//        resultant = AliasSubstitution(children[childIdx].Ptr32, p_typeTable);
//        resultant->DisposeChildren();
//
//        Toolbox::MemoryClean(children[childIdx].Ptr32);
//        children[childIdx].Ptr32 = resultant;
//    }
//
//    return p_typeNode->Clone();
//}
//----------------------------------------------------------------------------------------------
void TypeResolver::Specialize(TypeNode* p_specialization, TypeData& p_typeTemplate, TypeTable& p_typeTable)
{
    _ASSERTE(p_typeTable.find(p_specialization->FullName()) == p_typeTable.end());
    // 1. create entry for the specialized type by taking a clone from the template type
    TypeData& typeSpecialization    = p_typeTable[p_specialization->FullName()];
    typeSpecialization.Parents      = p_typeTemplate.Parents;
    typeSpecialization.TypeGraph    = p_typeTemplate.TypeGraph->Clone();

    // 2. substitute each template name with its corresponding p_specialization graph
    SpecializeAux(typeSpecialization.TypeGraph, p_specialization, p_typeTable);
}
//----------------------------------------------------------------------------------------------
void TypeResolver::SpecializeAux(TypeNode* p_targetType, TypeNode* p_specialization, TypeTable& p_typeTable)
{
    _ASSERTE(!p_specialization->TemplateArguments.empty());
    _ASSERTE(p_targetType->UserDefinedType == p_specialization->UserDefinedType);

    p_targetType->SpecializeChildren(p_targetType, p_specialization);
    p_targetType->SpecializeTemplateArguments(p_targetType, p_specialization);
}

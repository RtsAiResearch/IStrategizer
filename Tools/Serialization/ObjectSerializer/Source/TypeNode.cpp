#ifndef TYPENODE_H
	#include "TypeNode.h"
#endif

#include <cassert>
#include <string>
#include <algorithm>

unsigned LastTypeNodeId = 0;
unsigned TypeNodesCount = 0;

//----------------------------------------------------------------------------------------------
TypeNode::~TypeNode()
{
    for (int i = 0, size = Children.size(); i < size; ++i)
    {
        delete Children[i].Ptr32;
    }
    Children.clear();
    --TypeNodesCount;
    assert(TypeNodesCount >= 0);
}
//----------------------------------------------------------------------------------------------
TypeNode* TypeNode::Clone()
{
    TypeNode* clone = new TypeNode(Type);

    clone->Indirection          = Indirection;
    clone->UserDefinedType      = UserDefinedType;

    for(int i = 0; i < Children.size(); ++i)
    {
        if(Children[i].IsType)
        {
            clone->Children.push_back(TypeChild(Children[i].Ptr32->Clone()));
        }
        else
        {
            clone->Children.push_back(TypeChild(Children[i].Val32));
        }
    }

    for(int i = 0; i < TemplateArguments.size(); ++i)
    {
        clone->TemplateArguments.push_back(TemplateArguments[i]->Clone());
    }       

    return clone;
}
//----------------------------------------------------------------------------------------------
void TypeNode::SubstitueTypeName(const string& p_oldTypeName, const string& p_newTypeName)
{
    if (UserDefinedType == p_oldTypeName ||
        p_oldTypeName == "*")
    {   
        UserDefinedType = p_newTypeName;
    }

    for(int i = 0; i < Children.size(); ++i)
    {
        if(Children[i].IsType)
            Children[i].Ptr32->SubstitueTypeName(p_oldTypeName, p_newTypeName);
    }
}
//----------------------------------------------------------------------------------------------
void TypeNode::SpecializeChildren(TypeNode* p_template, TypeNode* p_specialization)
{
    vector<TypeNode*>&  templateArgs = p_template->TemplateArguments;
    vector<TypeNode*>&  specArgs     = p_specialization->TemplateArguments;

    assert(specArgs.size() >= templateArgs.size());

    for(int otherArgIdx = 0, argSize = templateArgs.size();
        otherArgIdx < argSize;
        ++otherArgIdx)
    {
        for(int myChildIdx = 0, childSize = Children.size();
            myChildIdx < childSize;
            ++myChildIdx)
        {
            if(!Children[myChildIdx].IsType)
                continue;

            TypeNode*& currentNode = Children[myChildIdx].Ptr32;

            if (currentNode->Children.empty() &&
                !currentNode->TemplateArguments.empty())
            {
                currentNode->SpecializeTemplateArguments(p_template, p_specialization);
            }
            else if (currentNode->Type == DTYPE_Template &&
                currentNode->UserDefinedType == templateArgs[otherArgIdx]->UserDefinedType)
            {
                assert(!currentNode->UserDefinedType.empty());
                delete currentNode;
                currentNode = specArgs[otherArgIdx]->Clone();
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
void TypeNode::SpecializeTemplateArguments(TypeNode* p_template, TypeNode* p_specialization)
{
    vector<TypeNode*>&  templateArgs = p_template->TemplateArguments;
    vector<TypeNode*>&  specArgs     = p_specialization->TemplateArguments;

    assert(specArgs.size() >= templateArgs.size());

    for(int otherArgIdx = 0, argSize = templateArgs.size();
        otherArgIdx < argSize;
        ++otherArgIdx)
    {
        for(int myArgIdx = 0, childSize = TemplateArguments.size();
            myArgIdx < childSize;
            ++myArgIdx)
        {
            TypeNode*& currentNode = TemplateArguments[myArgIdx];

            if (!currentNode->TemplateArguments.empty())
            {
                assert(currentNode->Children.empty());
                currentNode->SpecializeTemplateArguments(p_template, p_specialization);
            }
            else if (currentNode->Type == DTYPE_Template &&
                currentNode->UserDefinedType == templateArgs[otherArgIdx]->UserDefinedType)
            {
                assert(!currentNode->UserDefinedType.empty());
                delete currentNode;
                currentNode = specArgs[otherArgIdx]->Clone();
            }
        }
    }

    /*for(int myArgIdx = 0, myArgSize = TemplateArguments.size();
        myArgIdx < myArgSize;
        ++myArgIdx)
    {
        TypeNode*& myNode = TemplateArguments[myArgIdx];
        for(int otherArgIdx = 0, otherArgSize = p_templateArgs.size();
            otherArgIdx < otherArgSize;
            ++otherArgIdx)
        {
            TypeNode*& otherNode = p_templateArgs[otherArgIdx];
            if (myNode->Type == DTYPE_UserDefined &&
                myNode->UserDefinedType == otherNode->UserDefinedType)
            {
                delete myNode;
                myNode = p_specArgs[otherArgIdx]->Clone();
            }
            else
            {
                myNode->SpecializeTemplateArguments(p_templateArgs, p_specArgs);
            }
        }
    }*/
}
//----------------------------------------------------------------------------------------------
void TypeNode::SetTemplateArguments(vector<TypeNode*>& p_templateArguments)
{
    for(int templateIdx = 0; templateIdx < p_templateArguments.size(); ++templateIdx)
    {
        if (Type == DTYPE_UserDefined &&
            UserDefinedType == p_templateArguments[templateIdx]->UserDefinedType)
        {
            Type = DTYPE_Template;
        }

        for(int argIdx = 0; argIdx < TemplateArguments.size(); ++argIdx)
        {
            TemplateArguments[argIdx]->SetTemplateArguments(p_templateArguments);
        }

        for(int childIdx = 0; childIdx < Children.size(); ++childIdx)
        {
            if(!Children[childIdx].IsType)
                continue;

            Children[childIdx].Ptr32->SetTemplateArguments(p_templateArguments);
        }
    }
}
//----------------------------------------------------------------------------------------------
void TypeNode::PromoteChildren(bool p_setAsTemplates)
{
    assert(!Children.empty());

    TemplateArguments.resize(Children.size());
    for(int i = 0; i < Children.size(); ++i)
    {
        TemplateArguments[i] = Children[i].Ptr32;
        if(p_setAsTemplates)
            TemplateArguments[i]->Type = DTYPE_Template;
    }
    Children.clear();
}
//----------------------------------------------------------------------------------------------
void TypeNode::DisposeChildren()
{
    for(int childIdx = 0, childSize = Children.size();
        childIdx < childSize;
        ++childIdx)
    {
        if(Children[childIdx].IsType && Children[childIdx].Ptr32 != NULL)
            delete Children[childIdx].Ptr32;
    }

    Children.clear();
}
//----------------------------------------------------------------------------------------------
void TypeNode::Write(TypeNode* p_root, fstream& p_pen)
{
    int                 childrenCount;
    char                buffer[MaxTypeNameLength + 1];
    // true=type false=value
    bool                isType;
    TypeChild           current(p_root);
    queue<TypeChild>    q;

    q.push(current);

    while(!q.empty())
    {
        current = q.front();
        q.pop();

        if(current.IsType)
        {
            vector<TypeChild>& children = current.Ptr32->Children;
            childrenCount = children.size();
            for(int i = 0; i < childrenCount; ++i)
            {
                q.push(children[i]);
            }
            isType = true;
            p_pen.write(reinterpret_cast<char*>(&isType), sizeof(bool));
            p_pen.write(reinterpret_cast<char*>(&current.Ptr32->Type), sizeof(DataType));
            p_pen.write(reinterpret_cast<char*>(&current.Ptr32->Indirection), sizeof(bool));

            assert(current.Ptr32->UserDefinedType.size() <= MaxTypeNameLength);
            strcpy(buffer, current.Ptr32->UserDefinedType.c_str());
            p_pen.write(reinterpret_cast<char*>(buffer), sizeof(char) * (MaxTypeNameLength + 1));

            WriteTemplateData(current.Ptr32, p_pen);

            p_pen.write(reinterpret_cast<char*>(&childrenCount), sizeof(int));
        }
        else
        {
            isType = false;
            p_pen.write(reinterpret_cast<char*>(&isType), sizeof(bool));
            p_pen.write(reinterpret_cast<char*>(&current.Val32), sizeof(unsigned));
        }
    }
}
//----------------------------------------------------------------------------------------------
TypeNode* TypeNode::Read(fstream& p_eye)
{
    int                 childrenCount;
    char                buffer[MaxTypeNameLength + 1];
    // true=type false=value
    bool                isType = false;
    queue<TypeChild*>   q;
    TypeChild*          current;
    TypeChild*          virtualRoot;

    q.push(new TypeChild(reinterpret_cast<TypeNode*>(NULL)));
    virtualRoot = q.front();

    while(!q.empty() && p_eye)
    {
        current = q.front();
        q.pop();

        p_eye.read(reinterpret_cast<char*>(&isType), sizeof(bool));
        if(isType)
        {
            current->Ptr32 = new TypeNode();
            current->IsType = true;
            p_eye.read(reinterpret_cast<char*>(&current->Ptr32->Type), sizeof(DataType));
            p_eye.read(reinterpret_cast<char*>(&current->Ptr32->Indirection), sizeof(bool));

            p_eye.read(reinterpret_cast<char*>(buffer), sizeof(char) * (MaxTypeNameLength + 1));
            current->Ptr32->UserDefinedType = buffer;

            ReadTemplateData(current->Ptr32, p_eye);

            p_eye.read(reinterpret_cast<char*>(&childrenCount), sizeof(int));
            current->Ptr32->Children.resize(childrenCount);

            vector<TypeChild>& children = current->Ptr32->Children;
            for(int i = 0; i < childrenCount; ++i)
            {
                q.push(&children[i]);
            }
        }
        else
        {
            p_eye.read(reinterpret_cast<char*>(&current->Val32), sizeof(unsigned));
        }
    }

    TypeNode* root = virtualRoot->Ptr32;
    delete virtualRoot;
    return root;
}
//----------------------------------------------------------------------------------------------
void TypeNode::WriteTemplateData(TypeNode* p_typeNode, fstream& p_pen)
{
    vector<TypeNode*>& spec = p_typeNode->TemplateArguments;
    int specArgCount = spec.size();

    p_pen.write(reinterpret_cast<char*>(&specArgCount), sizeof(int));
    for(int argIdx = 0; argIdx < specArgCount; ++argIdx)
    {
        Write(spec[argIdx], p_pen);
    }
}
//----------------------------------------------------------------------------------------------
void TypeNode::ReadTemplateData(TypeNode* p_typeNode, fstream& p_eye)
{
    vector<TypeNode*>& spec = p_typeNode->TemplateArguments;
    int specArgCount;

    p_eye.read(reinterpret_cast<char*>(&specArgCount), sizeof(int));
    if(specArgCount > 0)
    {
        spec.resize(specArgCount);
        for(int argIdx = 0; argIdx < specArgCount; ++argIdx)
        {
            spec[argIdx] = Read(p_eye);
        }
    }
    
}
//----------------------------------------------------------------------------------------------
string TypeNode::FullName()
{
    string fullName;
    FullNameAux(fullName, true, false);
    return fullName;
}
//----------------------------------------------------------------------------------------------
void TypeNode::FullNameAux(string& p_str, bool p_templateFlag, bool p_childrenFlag)
{
    if(Type == DTYPE_UserDefined || Type == DTYPE_Template)
    {
        assert(!UserDefinedType.empty());
        p_str += UserDefinedType;
    }
    else
    {
        p_str += ToString(Type);
    }

    if(p_templateFlag)
    {
        if(!TemplateArguments.empty())
        {
            p_str += '(';
            for(int i = 0; i < TemplateArguments.size(); ++i)
            {
                //if(TemplateArguments[i]->Type == DTYPE_UserDefined)
                    TemplateArguments[i]->FullNameAux(p_str, true, false);
                //else
                //    TemplateArguments[i]->FullNameAux(p_str, false, true);

                if(i < TemplateArguments.size() - 1)
                    p_str += ',';
            }
            p_str += ')';
        }
    }

    if(p_childrenFlag)
    {
        if(!Children.empty())
        {
            p_str += '(';
            for(int i = 0; i < Children.size(); ++i)
            {
                if(Children[i].IsType)
                {
                    Children[i].Ptr32->FullNameAux(p_str, p_templateFlag, p_childrenFlag);
                }
                else
                {
                    p_str += Children[i].Val32 + '0';
                }
                if(i < Children.size() - 1)
                    p_str += ',';
            }
            p_str += ')';
        }
    }

    if(Indirection)
        p_str += '*';
}
//----------------------------------------------------------------------------------------------
string  TypeNode::ToString(DataType p_type)
{
    switch(Type)
    {
    case DTYPE_Bool:
        return "bool";
    case DTYPE_Char:
        return "char";
    case DTYPE_Short:
        return "char";
    case DTYPE_Int:
        return "int";
    case DTYPE_Float:
        return "float";
    case DTYPE_Double:
        return "double";
    case DTYPE_Array:
        return "array";
    case DTYPE_Pair:
        return "pair";
    case DTYPE_Vector:
        return "vector";
    case DTYPE_Map:
        return "map";
    case DTYPE_Set:
        return "set";
    default:
        assert(false);
    }

    return "";
}

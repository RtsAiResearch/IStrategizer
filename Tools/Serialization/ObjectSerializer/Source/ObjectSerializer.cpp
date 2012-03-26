#ifndef OBJECTSERIALIZER_H
    #include "ObjectSerializer.h"
#endif

#ifndef OBJECTFACTORY_H
    #include "ObjectFactory.h"
#endif

#ifndef SVECTOR_H
	#include "SVector.h"
#endif

#include <fstream>
#include <cassert>
#include <algorithm>

//----------------------------------------------------------------------------------------------
ObjectSerializer::ObjectSerializer()
{
    InitializeDataTypes();
    InitializeTypeTable();
}
//----------------------------------------------------------------------------------------------
void ObjectSerializer::InitializeTypeTable()
{
    if(m_typeTable.empty())
    {
        g_ObjectFormatter.ReadTypeTable(m_typeTable);
    }
}
//----------------------------------------------------------------------------------------------
void ObjectSerializer::InitializeDataTypes()
{
    m_basicTypeSize[DTYPE_Bool]   = sizeof(bool);
    m_basicTypeSize[DTYPE_Char]   = sizeof(char);
    m_basicTypeSize[DTYPE_Short]  = sizeof(short);
    m_basicTypeSize[DTYPE_Int]    = sizeof(int);
    m_basicTypeSize[DTYPE_Float]  = sizeof(float);
    m_basicTypeSize[DTYPE_Double] = sizeof(double);
};
//----------------------------------------------------------------------------------------------
void ObjectSerializer::PerformLateBinding( UserObject* p_object, TypeNode*& p_type )
{
    string candidateChild       = g_ObjectFactory.FromCName(p_object->CName());
    string candidateAncestor    = p_type->FullName();

    if (candidateChild != candidateAncestor &&
        IsAncestor(candidateAncestor, candidateChild))
    {
        p_type = m_typeTable[candidateChild].TypeGraph;
    }
}
//----------------------------------------------------------------------------------------------
bool ObjectSerializer::IsAncestor( const string& candidateAncestor, const string& candidateChild )
{
    vector<TypeNode*>&  parents     = m_typeTable[candidateChild].Parents;
    bool                foundParent = false;

    for(int pIdx = 0, pSize = parents.size(); pIdx < pSize; ++pIdx)
    {
        if(parents[pIdx]->FullName() == candidateAncestor)
        {
            foundParent = true;
            break;
        }
    }

    // Base Case: the candidate ancestor is a direct parent
    if(foundParent)
    {
        return true;
    }
    // Inductive Case: the candidate ancestor is ancestor to one of my parents
    else
    {
        for(int i = 0, size = parents.size(); i < size; ++i)
        {
            if(IsAncestor(candidateAncestor, parents[i]->FullName()))
                return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------------------------
void ObjectSerializer::Serialize(UserObject* p_object, string p_objectFileName)
{
    fstream pen;
    pen.open(p_objectFileName.c_str(), ios::binary | ios::out);
    assert(pen.is_open());

    const string& typeName = g_ObjectFactory.FromCName(p_object->CName());
    assert(m_typeTable.find(typeName) != m_typeTable.end());

    TypeData& typeData = m_typeTable[typeName];
    SerializeType(reinterpret_cast<char*>(p_object), typeData.TypeGraph, pen);

    pen.close();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeType(char* p_fieldAddress, TypeNode* p_type, fstream& p_pen)
{
    switch(p_type->Type)
    {
    case DTYPE_Array:
        return SerializeArray(p_fieldAddress, p_type, p_pen);

    case DTYPE_String:
        return SerializeString(p_fieldAddress, p_type, p_pen);

    case DTYPE_Vector:
        return SerializeContainerVector(p_fieldAddress, p_type, p_pen);

    case DTYPE_Map:
        return SerializeContainerMap(p_fieldAddress, p_type, p_pen);

    case DTYPE_Set:
        return SerializeContainerSet(p_fieldAddress, p_type, p_pen);

    case DTYPE_Pair:
        return SerializePair(p_fieldAddress, p_type, p_pen);

    case DTYPE_UserDefined:
        {
            string fullName = p_type->FullName();
            if(p_type->Indirection)
                fullName.erase(fullName.size() - 1, 1);

            assert(m_typeTable.find(fullName) != m_typeTable.end());
            return SerializeUserDefinedType(p_fieldAddress, m_typeTable[fullName].TypeGraph, p_type->Indirection, p_pen);
        }
    case DTYPE_Bool:
    case DTYPE_Char:
    case DTYPE_Short:
    case DTYPE_Int:
    case DTYPE_Float:
    case DTYPE_Double:
        return SerializeBasicType(p_fieldAddress, p_type, p_pen);
    }

    // should not reach here
    assert(false);

    return 0;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeBasicType(char* p_fieldAddress, TypeNode* p_type, fstream& p_pen)
{
    int size = m_basicTypeSize[p_type->Type];
    bool isNull = false;
    assert(p_type->Children.size() == 0);

    // Continuous pointers to scattered basic-type data
    if(p_type->Indirection)
    {
        unsigned* addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);
        isNull = (*addr32 == NULL);
        p_pen.write(reinterpret_cast<char*>(&isNull), sizeof(bool));
        if(!isNull)
        {
            p_fieldAddress = reinterpret_cast<char*>(*addr32);
            p_pen.write(p_fieldAddress, size);
        }
        size = sizeof(unsigned);
    }
    // Continuous basic-type data values
    else
    {
        p_pen.write(p_fieldAddress, size);
    }

    return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeUserDefinedType(char* p_fieldAddress, TypeNode* p_type, bool p_isPtr, fstream& p_pen)
{ 
    int     size = 0;
    UserObject* object = NULL;
    bool    isNull = false;
    if(p_isPtr)
    {
        int         length = 0;
        string      typeName;
        char        buffer[MaxTypeNameLength + 1];
        unsigned*   addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);

        isNull = (NULL == *addr32);
        p_pen.write(reinterpret_cast<char*>(&isNull), sizeof(bool));

        if(!isNull)
        {
            object = reinterpret_cast<UserObject*>(*addr32);

            typeName = g_ObjectFactory.FromCName(object->CName());

            PerformLateBinding(object, p_type);

            length = typeName.size();
            assert(length <= MaxTypeNameLength);
            strcpy(buffer, typeName.c_str());
            buffer[length] = 0;
            p_pen.write(buffer, MaxTypeNameLength + 1);

            object->InitializeAddresses();

            Iterator* addresses = object->GetIterator();
            unsigned* addr32;
            for(int memberIdx = 0; addresses->MoveNext(); ++memberIdx)    
            {
                assert(memberIdx < p_type->Children.size());
                addr32 = reinterpret_cast<unsigned*>(addresses->Current());
                SerializeType(reinterpret_cast<char*>(*addr32), p_type->Children[memberIdx].Ptr32, p_pen);
            }
        }
        size = sizeof(unsigned);
    }
    else
    {
        object = reinterpret_cast<UserObject*>(p_fieldAddress);
        object->InitializeAddresses();

        Iterator* addresses = object->GetIterator();
        unsigned* addr32;
        for(int memberIdx = 0; addresses->MoveNext(); ++memberIdx)    
        {
            assert(memberIdx < p_type->Children.size());
            addr32 = reinterpret_cast<unsigned*>(addresses->Current());
            SerializeType(reinterpret_cast<char*>(*addr32), p_type->Children[memberIdx].Ptr32, p_pen);
        }

        size = object->TypeSize();
    }

    return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeArray(char* p_fieldAddress, TypeNode* p_type, fstream& p_pen)
{
    // not supported for this time
    assert(false);

    int         size = 0;
    int         length = p_type->Children[0].Val32;
    TypeNode*   type = p_type->Children[1].Ptr32;
    bool        isNull = false;

    assert(p_type->Children.size() == 2);

    if(p_type->Indirection)
    {
        unsigned* addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);
        isNull = (*addr32 == NULL);
        p_pen.write(reinterpret_cast<char*>(&isNull), sizeof(bool));

        if(!isNull)
        {
            for(int i = 0; i < length; ++i)
            {
                p_fieldAddress = reinterpret_cast<char*>(*addr32);
                SerializeType(p_fieldAddress, type, p_pen);
                addr32++;
            }
        }

        size = sizeof(unsigned) * length;
    }
    else
    {
        int s = 0;
        for(int i = 0; i < length; ++i)
        {
            s = SerializeType(p_fieldAddress, type, p_pen);
            p_fieldAddress += s;
            size += s;
        }
    }

    return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeString(char* p_fieldAddress, TypeNode* p_type, fstream& p_pen)
{
    int     length = 0;
    int     size = NULL;
    char*   buffer = NULL;
    string* str = NULL;
    bool    isNull = false;

    if(p_type->Indirection)
    {
        unsigned* addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);
        isNull = (*addr32 == NULL);
        p_pen.write(reinterpret_cast<char*>(&isNull), sizeof(bool));

        if(!isNull)
        {   
            str = reinterpret_cast<string*>(*addr32);
            length = str->size();
            p_pen.write(reinterpret_cast<char*>(&length), sizeof(int));
            buffer = const_cast<char*>(str->c_str());
            p_pen.write(reinterpret_cast<char*>(buffer), sizeof(char) * length);
        }

        size = sizeof(unsigned);
    }
    else
    {
        str = reinterpret_cast<string*>(p_fieldAddress);
        length = str->size();
        p_pen.write(reinterpret_cast<char*>(&length), sizeof(int));
        buffer = const_cast<char*>(str->c_str());
        p_pen.write(reinterpret_cast<char*>(buffer), sizeof(char) * length);

        size = sizeof(string);
    }

    return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeContainerVector(char* p_fieldAddress, TypeNode* p_type, fstream& p_pen)
{
    // Pointer to vector is not supported
    assert(p_type->Indirection == false);
    assert(p_type->TemplateArguments.size() == 1);

    Container*  container   = reinterpret_cast<Container*>(p_fieldAddress);
    Iterator*   itr         = container->GetIterator();
    int         count       = container->ContainerCount();

    p_pen.write(reinterpret_cast<char*>(&count), sizeof(int));
    while(itr->MoveNext())
    {
        SerializeType(itr->Current(), p_type->TemplateArguments[0], p_pen);
    }

    return container->TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeContainerMap(char* p_fieldAddress, TypeNode* p_type, fstream& p_pen)
{
    // Pointer to map is not supported
    assert(p_type->Indirection == false);
    assert(p_type->TemplateArguments.size() == 1);

    Container*  container   = reinterpret_cast<Container*>(p_fieldAddress);
    Iterator*   itr         = container->GetIterator();
    int         count       = container->ContainerCount();

    p_pen.write(reinterpret_cast<char*>(&count), sizeof(int));
    while(itr->MoveNext())
    {
        SerializeType(itr->Current(), p_type->TemplateArguments[0], p_pen);
    }

    return container->TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeContainerSet(char* p_fieldAddress, TypeNode* p_type, fstream& p_pen)
{
    // Pointer to set is not supported
    assert(p_type->Indirection == false);
    assert(p_type->TemplateArguments.size() == 1);

    Container*  container   = reinterpret_cast<Container*>(p_fieldAddress);
    Iterator*   itr         = container->GetIterator();
    int         count       = container->ContainerCount();

    p_pen.write(reinterpret_cast<char*>(&count), sizeof(int));
    while(itr->MoveNext())
    {
        SerializeType(itr->Current(), p_type->TemplateArguments[0], p_pen);
    }

    return container->TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializePair(char* p_fieldAddress, TypeNode* p_type, fstream& p_pen)
{
    // Pointer to pair is not supported
    assert(p_type->Indirection == false);
    assert(p_type->TemplateArguments.size() == 2);

    UserObject* object = reinterpret_cast<UserObject*>(p_fieldAddress);
    object->InitializeAddresses();

    Iterator* addresses = object->GetIterator();
    unsigned* addr32;
    for(int memberIdx = 0; addresses->MoveNext(); ++memberIdx)    
    {
        assert(memberIdx < p_type->TemplateArguments.size());
        addr32 = reinterpret_cast<unsigned*>(addresses->Current());
        SerializeType(reinterpret_cast<char*>(*addr32), p_type->TemplateArguments[memberIdx], p_pen);
    }

    return object->TypeSize();
}
//----------------------------------------------------------------------------------------------
void ObjectSerializer::Deserialize(UserObject* p_object, string p_objectFileName)
{
    fstream eye;
    eye.open(p_objectFileName.c_str(), ios::binary | ios::in);
    assert(eye.is_open());

    const string& typeName = g_ObjectFactory.FromCName(p_object->CName());
    assert(m_typeTable.find(typeName) != m_typeTable.end());

    TypeData& typeData = m_typeTable[typeName];
    DeserializeType(reinterpret_cast<char*>(p_object), typeData.TypeGraph, eye);

    eye.close();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeType(char* p_fieldAddress, TypeNode* p_type, fstream& p_eye)
{
    switch(p_type->Type)
    {
    case DTYPE_Array:
        return DeserializeArray(p_fieldAddress, p_type, p_eye);

    case DTYPE_String:
        return DeserializeString(p_fieldAddress, p_type, p_eye);

    case DTYPE_Vector:
        return DeserializeContainerVector(p_fieldAddress, p_type, p_eye);

    case DTYPE_Map:
        return DeserializeContainerMap(p_fieldAddress, p_type, p_eye);
    
    case DTYPE_Set:
        return DeserializeContainerSet(p_fieldAddress, p_type, p_eye);

    case DTYPE_Pair:
        return DeserializePair(p_fieldAddress, p_type, p_eye);

    case DTYPE_UserDefined:
        {
            string fullName = p_type->FullName();
            if(p_type->Indirection)
                fullName.erase(fullName.size() - 1, 1);

            assert(m_typeTable.find(fullName) != m_typeTable.end());
            return DeserializeUserDefinedType(p_fieldAddress, m_typeTable[fullName].TypeGraph, p_type->Indirection, p_eye);
        }

    case DTYPE_Bool:
    case DTYPE_Char:
    case DTYPE_Short:
    case DTYPE_Int:
    case DTYPE_Float:
    case DTYPE_Double:
        return DeserializeBasicType(p_fieldAddress, p_type, p_eye);
    }

    return 0;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeBasicType(char* p_fieldAddress, TypeNode* p_type, fstream& p_eye)
{
    int size = m_basicTypeSize[p_type->Type];
    bool isNull = false;

    if(p_type->Indirection)
    {
        p_eye.read(reinterpret_cast<char*>(&isNull), sizeof(bool));
        char* memChunk = NULL;
        if(!isNull)
        {
            memChunk = new char[size];
            p_eye.read(memChunk, size);
        }

        unsigned* addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);
        *addr32 = reinterpret_cast<unsigned>(memChunk);

        size = sizeof(unsigned);
    }
    else
    {   
        p_eye.read(p_fieldAddress, size);
    }

    return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeUserDefinedType(char* p_fieldAddress, TypeNode* p_type, bool p_isPtr, fstream& p_eye)
{ 
    int     size = 0;
    UserObject* object = NULL;
    bool    isNull = false;

    if(p_isPtr)
    {
        unsigned* addr32;
        p_eye.read(reinterpret_cast<char*>(&isNull), sizeof(bool));
        addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);

        if(!isNull)
        {
            string  typeName;
            char    buffer[MaxTypeNameLength + 1];

            p_eye.read(buffer, MaxTypeNameLength + 1);
            typeName = buffer;
            object = static_cast<UserObject*>(g_ObjectFactory.GetObject(typeName)->Prototype());

            PerformLateBinding(object, p_type);

            object->InitializeAddresses();

            Iterator* addresses = object->GetIterator();
            unsigned* addr32;
            for(int memberIdx = 0; addresses->MoveNext(); ++memberIdx)    
            {
                assert(memberIdx < p_type->Children.size());
                addr32 = reinterpret_cast<unsigned*>(addresses->Current());
                DeserializeType(reinterpret_cast<char*>(*addr32), p_type->Children[memberIdx].Ptr32, p_eye);
            }
        }
        
        *addr32 = reinterpret_cast<unsigned>(object);
        size = sizeof(unsigned);
    }
    else
    {
        object = reinterpret_cast<UserObject*>(p_fieldAddress);
        object->InitializeAddresses();
    
        Iterator* addresses = object->GetIterator();
        unsigned* addr32;
        for(int memberIdx = 0; addresses->MoveNext(); ++memberIdx)    
        {
            assert(memberIdx < p_type->Children.size());
            addr32 = reinterpret_cast<unsigned*>(addresses->Current());
            DeserializeType(reinterpret_cast<char*>(*addr32), p_type->Children[memberIdx].Ptr32, p_eye);
        }

        size = object->TypeSize();
    }

    return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeArray(char* p_fieldAddress, TypeNode* p_type, fstream& p_eye)
{
    int         size = 0;
    int         length = p_type->Children[0].Val32;
    TypeNode*   type = p_type->Children[1].Ptr32;
    bool        isNull = false;

    if(p_type->Indirection)
    {
        unsigned* addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);
        *addr32 = NULL;

        p_eye.read(reinterpret_cast<char*>(&isNull), sizeof(bool));
        if(!isNull)
        {
            for(int i = 0; i < length; ++i)
            {
                p_fieldAddress = reinterpret_cast<char*>(*addr32);
                DeserializeType(p_fieldAddress, type, p_eye);
                addr32++;
            }
        }

        size = sizeof(unsigned) * length;
    }
    else
    {
        int s = 0;
        for(int i = 0; i < length; ++i)
        {
            s = DeserializeType(p_fieldAddress, type, p_eye);
            p_fieldAddress += s;
            size += s;
        }
    }

    return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeString(char* p_fieldAddress, TypeNode* p_type, fstream& p_eye)
{
    int     size = 0;
    int     length = 0;
    char*   buffer = NULL;
    string* str = NULL;
    bool    isNull = false;

    if(p_type->Indirection)
    {
        unsigned* addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);
        p_eye.read(reinterpret_cast<char*>(&isNull), sizeof(bool));

        if(!isNull)
        {
            p_eye.read(reinterpret_cast<char*>(&length), sizeof(int));
            buffer = new char[length + 1];
            p_eye.read(reinterpret_cast<char*>(buffer), sizeof(char) * length);
            buffer[length] = 0;

            str = new string(buffer);
            addr32 = reinterpret_cast<unsigned*>(p_fieldAddress);
        }

        *addr32 = reinterpret_cast<unsigned>(str);
        size = sizeof(unsigned);
    }
    else
    {
        p_eye.read(reinterpret_cast<char*>(&length), sizeof(int));
        buffer = new char[length + 1];
        p_eye.read(reinterpret_cast<char*>(buffer), sizeof(char) * length);
        buffer[length] = 0;

        str = reinterpret_cast<string*>(p_fieldAddress);
        *str = buffer;
        size = sizeof(string);
    }

    if(buffer != NULL)
        delete[] buffer;

    return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeContainerVector(char* p_fieldAddress, TypeNode* p_type, fstream& p_eye)
{
    // Pointer to vector is not supported
    assert(p_type->Indirection == false);

    Container* container = reinterpret_cast<Container*>(p_fieldAddress);
    int count;

    p_eye.read(reinterpret_cast<char*>(&count), sizeof(int));
    container->Clear();

    char* tempStorage = container->GetTemp();
    for(int i = 0; i < count; ++i)
    {
        DeserializeType(tempStorage, p_type->TemplateArguments[0], p_eye);
        container->AddTemp();
    }

    return container->TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeContainerMap(char* p_fieldAddress, TypeNode* p_type, fstream& p_eye)
{
    // Pointer to map is not supported
    assert(p_type->Indirection == false);

    Container* container = reinterpret_cast<Container*>(p_fieldAddress);
    int count;

    p_eye.read(reinterpret_cast<char*>(&count), sizeof(int));
    container->Clear();

    char* tempStorage = container->GetTemp();
    for(int i = 0; i < count; ++i)
    {
        DeserializeType(tempStorage, p_type->TemplateArguments[0], p_eye);
        container->AddTemp();
    }

    return container->TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeContainerSet(char* p_fieldAddress, TypeNode* p_type, fstream& p_eye)
{
    // Pointer to set is not supported
    assert(p_type->Indirection == false);

    Container* container = reinterpret_cast<Container*>(p_fieldAddress);
    int count;

    p_eye.read(reinterpret_cast<char*>(&count), sizeof(int));
    container->Clear();
    char* tempStorage0 = container->GetTemp();

    for(int i = 0; i < count; ++i)
    {
        DeserializeType(tempStorage0, p_type->TemplateArguments[0], p_eye);
        container->AddTemp();
    }

    return container->TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializePair(char* p_fieldAddress, TypeNode* p_type, fstream& p_eye)
{
    // Pointer to pair is not supported
    assert(p_type->Indirection == false);

    UserObject* object = reinterpret_cast<UserObject*>(p_fieldAddress);
    object->InitializeAddresses();

    Iterator* addresses = object->GetIterator();
    unsigned* addr32;
    for(int memberIdx = 0; addresses->MoveNext(); ++memberIdx)    
    {
        addr32 = reinterpret_cast<unsigned*>(addresses->Current());
        DeserializeType(reinterpret_cast<char*>(*addr32), p_type->TemplateArguments[memberIdx], p_eye);
    }

    return object->TypeSize();
}

#ifndef OBJECTSERIALIZER_H
#define OBJECTSERIALIZER_H

//#ifndef OBJECTFORMATTER_H
//    #include "ObjectFormatter.h"
//#endif
//
//#ifndef USEROBJECT_H
//    #include "UserObject.h"
//#endif
#include <fstream>

#include "TypeTable.h"

namespace Serialization
{
    class UserObject;

    class ObjectSerializer
    {
        TypeTable m_typeTable;
        int m_basicTypeSize[16];

    protected:
        void InitializeDataTypes();
        void InitializeTypeTable();

        int SerializeType(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_pen);
        int SerializeBasicType(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_pen);
        int SerializeUserDefinedType(char* p_fieldAddress, TypeNode* p_type, bool p_isPtr, std::fstream& p_pen);
        int SerializeArray(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_pen);
        int SerializeString(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_pen);
        int SerializeContainerVector(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_pen);
        int SerializeContainerMap(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_pen);
        int SerializeContainerSet(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_pen);
        int SerializePair(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_pen);

        int DeserializeType(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_eye);
        int DeserializeBasicType(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_eye);
        int DeserializeUserDefinedType(char* p_fieldAddress, TypeNode* p_type, bool p_isPtr, std::fstream& p_eye);
        int DeserializeArray(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_eye);
        int DeserializeString(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_eye);
        int DeserializeContainerVector(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_eye);
        int DeserializeContainerMap(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_eye);
        int DeserializeContainerSet(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_eye);
        int DeserializePair(char* p_fieldAddress, TypeNode* p_type, std::fstream& p_eye);

    public:
        ObjectSerializer();
        TypeTable& TypeTable() { return m_typeTable; }
        void Serialize(const UserObject* p_object, std::string p_objectFileName);
        void Deserialize(UserObject* p_object, std::string p_objectFileName);
        static ObjectSerializer& Instance() { static ObjectSerializer instance; return instance; }
        void PerformLateBinding( UserObject* p_object, TypeNode*& p_type );
        bool IsAncestor( const std::string& candidateAncestor, const std::string& candidateChild );
    };
}

#define g_ObjectSerializer Serialization::ObjectSerializer::Instance()

#endif // OBJECTSERIALIZER_H

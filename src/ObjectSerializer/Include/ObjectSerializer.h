#ifndef OBJECTSERIALIZER_H
#define OBJECTSERIALIZER_H

#include <fstream>
#include "TypeTable.h"
#include "ISerializable.h"

namespace Serialization
{
    class ObjectSerializer
    {
        TypeTable m_typeTable;
        int m_basicTypeSize[16];

    protected:
        void InitializeDataTypes();
        void InitializeTypeTable();

        int SerializeType(char* pMem, TypeNode* pType, std::fstream& pen);
        int SerializeBasicType(char* pMem, TypeNode* pType, std::fstream& pen);
        int SerializeUserDefinedType(ISerializable* pObj, TypeNode* pType, bool p_isPtr, std::fstream& pen);
        int SerializeArray(char* pMem, TypeNode* pType, std::fstream& pen);
        int SerializeString(char* pMem, TypeNode* pType, std::fstream& pen);
        int SerializeContainerVector(ISerializable* pObj ,TypeNode* pType, std::fstream& pen);
        int SerializeContainerMap(ISerializable* pObj, TypeNode* pType, std::fstream& pen);
        int SerializeContainerSet(ISerializable* pObj, TypeNode* pType, std::fstream& pen);
        int SerializePair(ISerializable* pObj, TypeNode* pType, std::fstream& pen);

        int DeserializeType(char* pMem, TypeNode* pType, std::fstream& eye);
        int DeserializeBasicType(char* pMem, TypeNode* pType, std::fstream& eye);
        int DeserializeUserDefinedType(ISerializable* pObj, TypeNode* pType, bool p_isPtr, std::fstream& eye);
        int DeserializeArray(char* pMem, TypeNode* pType, std::fstream& eye);
        int DeserializeString(char* pMem, TypeNode* pType, std::fstream& eye);
        int DeserializeContainerVector(ISerializable* pObj, TypeNode* pType, std::fstream& eye);
        int DeserializeContainerMap(ISerializable* pObj, TypeNode* pType, std::fstream& eye);
        int DeserializeContainerSet(ISerializable* pObj, TypeNode* pType, std::fstream& eye);
        int DeserializePair(ISerializable* pObj, TypeNode* pType, std::fstream& eye);

    public:
        ObjectSerializer();
        TypeTable& TypeTable() { return m_typeTable; }
        void Serialize(const ISerializable* p_object, std::string objectFileName);
        void Deserialize(ISerializable* pObj, std::string objectFileName);
        static ObjectSerializer& Instance() { static ObjectSerializer instance; return instance; }
        void PerformLateBinding(ISerializable* pObj, TypeNode*& pType );
        bool IsAncestor( const std::string& candidateAncestor, const std::string& candidateChild );
    };
}

#define g_ObjectSerializer Serialization::ObjectSerializer::Instance()

#endif // OBJECTSERIALIZER_H

#ifndef OBJECTSERIALIZER_H
#include "ObjectSerializer.h"
#endif

#ifndef OBJECTFACTORY_H
#include "ObjectFactory.h"
#endif

#ifndef SVECTOR_H
#include "SVector.h"
#endif
#include "TypeTable.h"
#include "ObjectFormatter.h"
#include "TypeNode.h"
#include <fstream>
#include <cassert>
#include <algorithm>

size_t Serialization::sm_lastSerializableObjID = 0;

//----------------------------------------------------------------------------------------------
ObjectSerializer::ObjectSerializer()
{
	InitializeDataTypes();
	InitializeTypeTable();
}
//----------------------------------------------------------------------------------------------
void ObjectSerializer::InitializeTypeTable()
{
	if (m_typeTable.empty())
	{
		g_ObjectFormatter.ReadTypeTable(m_typeTable);
	}
}
//----------------------------------------------------------------------------------------------
void ObjectSerializer::InitializeDataTypes()
{
	m_basicTypeSize[DTYPE_Bool] = sizeof(bool);
	m_basicTypeSize[DTYPE_Char] = sizeof(char);
	m_basicTypeSize[DTYPE_Short] = sizeof(short);
	m_basicTypeSize[DTYPE_Int] = sizeof(int);
	m_basicTypeSize[DTYPE_Unsigned] = sizeof(unsigned);
	m_basicTypeSize[DTYPE_Float] = sizeof(float);
	m_basicTypeSize[DTYPE_Double] = sizeof(double);
};
//----------------------------------------------------------------------------------------------
void ObjectSerializer::PerformLateBinding(ISerializable* pObj, TypeNode*& pType)
{
	auto objLayout = pObj->GetObjectLayout();
	string candidateChild = g_ObjectFactory.FromCName(objLayout.CName());
	string candidateAncestor = pType->FullName();

	if (candidateChild != candidateAncestor &&
		IsAncestor(candidateAncestor, candidateChild))
	{
		pType = m_typeTable[candidateChild].TypeGraph;
	}
}
//----------------------------------------------------------------------------------------------
bool ObjectSerializer::IsAncestor(const string& candidateAncestor, const string& candidateChild)
{
	vector<TypeNode*>&  parents = m_typeTable[candidateChild].Parents;
	bool                foundParent = false;

	for (int pIdx = 0, pSize = parents.size(); pIdx < pSize; ++pIdx)
	{
		if (parents[pIdx]->FullName() == candidateAncestor)
		{
			foundParent = true;
			break;
		}
	}

	// Base Case: the candidate ancestor is a direct parent
	if (foundParent)
	{
		return true;
	}
	// Inductive Case: the candidate ancestor is ancestor to one of my parents
	else
	{
		for (int i = 0, size = parents.size(); i < size; ++i)
		{
			if (IsAncestor(candidateAncestor, parents[i]->FullName()))
				return true;
		}
	}

	return false;
}
//----------------------------------------------------------------------------------------------
void ObjectSerializer::Serialize(const ISerializable* pObject, string objFilename)
{
	fstream pen;
	pen.open(objFilename.c_str(), ios::binary | ios::out);
	_ASSERTE(pen.is_open());

	auto objLayout = const_cast<ISerializable*>(pObject)->GetObjectLayout();
	const string& typeName = g_ObjectFactory.FromCName(objLayout.CName());
	_ASSERTE(m_typeTable.find(typeName) != m_typeTable.end());

	TypeData& typeData = m_typeTable[typeName];
	SerializeType(reinterpret_cast<char*>(const_cast<ISerializable*>(pObject)), typeData.TypeGraph, pen);

	pen.close();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeType(char* pMem, TypeNode* pType, fstream& pen)
{
	switch (pType->Type)
	{
	case DTYPE_Array:
		return SerializeArray(pMem, pType, pen);

	case DTYPE_String:
		return SerializeString(pMem, pType, pen);

	case DTYPE_Vector:
		return SerializeContainerVector((ISerializable*)pMem, pType, pen);

	case DTYPE_Map:
		return SerializeContainerMap((ISerializable*)pMem, pType, pen);

	case DTYPE_Set:
		return SerializeContainerSet((ISerializable*)pMem, pType, pen);

	case DTYPE_Pair:
		return SerializePair((ISerializable*)pMem, pType, pen);

	case DTYPE_UserDefined:
	{
							  string fullName = pType->FullName();
							  if (pType->Indirection)
								  fullName.erase(fullName.size() - 1, 1);

							  _ASSERTE(m_typeTable.find(fullName) != m_typeTable.end());

							  return SerializeUserDefinedType((ISerializable*)pMem, m_typeTable[fullName].TypeGraph, pType->Indirection, pen);
	}
	case DTYPE_Bool:
	case DTYPE_Char:
	case DTYPE_Short:
	case DTYPE_Int:
	case DTYPE_Unsigned:
	case DTYPE_Float:
	case DTYPE_Double:
		return SerializeBasicType(pMem, pType, pen);
	}

	// should not reach here
	_ASSERTE(false);

	return 0;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeBasicType(char* pMem, TypeNode* pType, fstream& pen)
{
	int size = m_basicTypeSize[pType->Type];
	bool isNull = false;
	_ASSERTE(pType->Children.size() == 0);

	// Continuous pointers to scattered basic-type data
	if (pType->Indirection)
	{
		unsigned* addr32 = reinterpret_cast<unsigned*>(pMem);
		isNull = (*addr32 == NULL);
		pen.write(reinterpret_cast<char*>(&isNull), sizeof(bool));
		if (!isNull)
		{
			pMem = reinterpret_cast<char*>(*addr32);
			pen.write(pMem, size);
		}
		size = sizeof(unsigned);
	}
	// Continuous basic-type data values
	else
	{
		pen.write(pMem, size);
	}

	return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeUserDefinedType(ISerializable *pObj, TypeNode* pType, bool isPtr, fstream& pen)
{
	int     size = 0;
	bool    isNull = false;

	if (isPtr)
	{
		// Access the pointer pointed by the pointer to pointer
		pObj = (*(ISerializable**)pObj);
		int         length = 0;
		string      typeName;
		char        buffer[MaxTypeNameLength + 1];

		isNull = (NULL == pObj);
		pen.write(reinterpret_cast<char*>(&isNull), sizeof(bool));

		if (!isNull)
		{
			auto        objLayout = pObj->GetObjectLayout();

			typeName = g_ObjectFactory.FromCName(objLayout.CName());

			PerformLateBinding(pObj, pType);

			length = typeName.size();
			_ASSERTE(length <= MaxTypeNameLength);
			strcpy_s(buffer, typeName.c_str());
			buffer[length] = 0;
			pen.write(buffer, MaxTypeNameLength + 1);

			Iterator* addresses = objLayout.GetIterator();
			unsigned* addr32;
			for (int memberIdx = 0; addresses->MoveNext(); ++memberIdx)
			{
				_ASSERTE(memberIdx < pType->Children.size());
				addr32 = reinterpret_cast<unsigned*>(addresses->Current());
				SerializeType(reinterpret_cast<char*>(*addr32), pType->Children[memberIdx].Ptr32, pen);
			}
		}
		size = sizeof(unsigned);
	}
	else
	{
		auto objLayout = pObj->GetObjectLayout();
		Iterator* addresses = objLayout.GetIterator();
		unsigned* addr32;
		for (int memberIdx = 0; addresses->MoveNext(); ++memberIdx)
		{
			_ASSERTE(memberIdx < pType->Children.size());
			addr32 = reinterpret_cast<unsigned*>(addresses->Current());
			SerializeType(reinterpret_cast<char*>(*addr32), pType->Children[memberIdx].Ptr32, pen);
		}

		size = objLayout.TypeSize();
	}

	return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeArray(char* pMem, TypeNode* pType, fstream& pen)
{
	// not supported for this time
	_ASSERTE(false);

	int         size = 0;
	int         length = pType->Children[0].Val32;
	TypeNode*   type = pType->Children[1].Ptr32;
	bool        isNull = false;

	_ASSERTE(pType->Children.size() == 2);

	if (pType->Indirection)
	{
		unsigned* addr32 = reinterpret_cast<unsigned*>(pMem);
		isNull = (*addr32 == NULL);
		pen.write(reinterpret_cast<char*>(&isNull), sizeof(bool));

		if (!isNull)
		{
			for (int i = 0; i < length; ++i)
			{
				pMem = reinterpret_cast<char*>(*addr32);
				SerializeType(pMem, type, pen);
				addr32++;
			}
		}

		size = sizeof(unsigned)* length;
	}
	else
	{
		int s = 0;
		for (int i = 0; i < length; ++i)
		{
			s = SerializeType(pMem, type, pen);
			pMem += s;
			size += s;
		}
	}

	return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeString(char* pMem, TypeNode* pType, fstream& pen)
{
	int     length = 0;
	int     size = NULL;
	char*   buffer = NULL;
	string* str = NULL;
	bool    isNull = false;

	if (pType->Indirection)
	{
		unsigned* addr32 = reinterpret_cast<unsigned*>(pMem);
		isNull = (*addr32 == NULL);
		pen.write(reinterpret_cast<char*>(&isNull), sizeof(bool));

		if (!isNull)
		{
			str = reinterpret_cast<string*>(*addr32);
			length = str->size();
			pen.write(reinterpret_cast<char*>(&length), sizeof(int));
			buffer = const_cast<char*>(str->c_str());
			pen.write(reinterpret_cast<char*>(buffer), sizeof(char)* length);
		}

		size = sizeof(unsigned);
	}
	else
	{
		str = reinterpret_cast<string*>(pMem);
		length = str->size();
		pen.write(reinterpret_cast<char*>(&length), sizeof(int));
		buffer = const_cast<char*>(str->c_str());
		pen.write(reinterpret_cast<char*>(buffer), sizeof(char)* length);

		size = sizeof(string);
	}

	return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeContainerVector(ISerializable* pObj, TypeNode* pType, fstream& pen)
{
	// Pointer to vector is not supported
	_ASSERTE(pType->Indirection == false);
	_ASSERTE(pType->TemplateArguments.size() == 1);

	auto		objLayout = pObj->GetObjectLayout();
	IContainer* pContainer = dynamic_cast<IContainer*>(pObj);
	_ASSERTE(pContainer);
	Iterator*   itr = objLayout.GetIterator();
	int         count = pContainer->ContainerCount();

	pen.write(reinterpret_cast<char*>(&count), sizeof(int));
	while (itr->MoveNext())
	{
		SerializeType(itr->Current(), pType->TemplateArguments[0], pen);
	}

	return objLayout.TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeContainerMap(ISerializable* pObj, TypeNode* pType, fstream& pen)
{
	// Pointer to map is not supported
	_ASSERTE(pType->Indirection == false);
	_ASSERTE(pType->TemplateArguments.size() == 1);

	auto        objLayout = pObj->GetObjectLayout();
	IContainer* pContainer = dynamic_cast<IContainer*>(pObj);
	_ASSERTE(pContainer);
	Iterator*   itr = objLayout.GetIterator();
	int         count = pContainer->ContainerCount();

	pen.write(reinterpret_cast<char*>(&count), sizeof(int));
	while (itr->MoveNext())
	{
		SerializeType(itr->Current(), pType->TemplateArguments[0], pen);
	}

	return objLayout.TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializeContainerSet(ISerializable* pObj, TypeNode* pType, fstream& pen)
{
	// Pointer to set is not supported
	_ASSERTE(pType->Indirection == false);
	_ASSERTE(pType->TemplateArguments.size() == 1);

	auto        objLayout = pObj->GetObjectLayout();
	IContainer* pContainer = dynamic_cast<IContainer*>(pObj);
	_ASSERTE(pContainer);
	Iterator*   itr = objLayout.GetIterator();
	int         count = pContainer->ContainerCount();

	pen.write(reinterpret_cast<char*>(&count), sizeof(int));
	while (itr->MoveNext())
	{
		SerializeType(itr->Current(), pType->TemplateArguments[0], pen);
	}

	return objLayout.TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::SerializePair(ISerializable* pObj, TypeNode* pType, fstream& pen)
{
	// Pointer to pair is not supported
	_ASSERTE(pType->Indirection == false);
	_ASSERTE(pType->TemplateArguments.size() == 2);

	auto        objLayout = pObj->GetObjectLayout();

	Iterator* addresses = objLayout.GetIterator();
	unsigned* addr32;
	for (int memberIdx = 0; addresses->MoveNext(); ++memberIdx)
	{
		_ASSERTE(memberIdx < pType->TemplateArguments.size());
		addr32 = reinterpret_cast<unsigned*>(addresses->Current());
		SerializeType(reinterpret_cast<char*>(*addr32), pType->TemplateArguments[memberIdx], pen);
	}

	return objLayout.TypeSize();
}
//----------------------------------------------------------------------------------------------
void ObjectSerializer::Deserialize(ISerializable* pObj, string objectFileName)
{
	fstream eye;
	eye.open(objectFileName.c_str(), ios::binary | ios::in);
	_ASSERTE(eye.is_open());
	_ASSERTE(pObj);

	auto objLayout = pObj->GetObjectLayout();
	const string& typeName = g_ObjectFactory.FromCName(objLayout.CName());
	_ASSERTE(m_typeTable.find(typeName) != m_typeTable.end());

	TypeData& typeData = m_typeTable[typeName];
	DeserializeType(reinterpret_cast<char*>(pObj), typeData.TypeGraph, eye);

	eye.close();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeType(char* pMem, TypeNode* pType, fstream& eye)
{
	switch (pType->Type)
	{
	case DTYPE_Array:
		return DeserializeArray(pMem, pType, eye);

	case DTYPE_String:
		return DeserializeString(pMem, pType, eye);

	case DTYPE_Vector:
		return DeserializeContainerVector((ISerializable*)pMem, pType, eye);

	case DTYPE_Map:
		return DeserializeContainerMap((ISerializable*)pMem, pType, eye);

	case DTYPE_Set:
		return DeserializeContainerSet((ISerializable*)pMem, pType, eye);

	case DTYPE_Pair:
		return DeserializePair((ISerializable*)pMem, pType, eye);

	case DTYPE_UserDefined:
	{
							  string fullName = pType->FullName();
							  if (pType->Indirection)
								  fullName.erase(fullName.size() - 1, 1);

							  _ASSERTE(m_typeTable.find(fullName) != m_typeTable.end());
							  return DeserializeUserDefinedType((ISerializable*)pMem, m_typeTable[fullName].TypeGraph, pType->Indirection, eye);
	}

	case DTYPE_Bool:
	case DTYPE_Char:
	case DTYPE_Short:
	case DTYPE_Int:
	case DTYPE_Unsigned:
	case DTYPE_Float:
	case DTYPE_Double:
		return DeserializeBasicType(pMem, pType, eye);
	}

	return 0;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeBasicType(char* pMem, TypeNode* pType, fstream& eye)
{
	int size = m_basicTypeSize[pType->Type];
	bool isNull = false;

	if (pType->Indirection)
	{
		eye.read(reinterpret_cast<char*>(&isNull), sizeof(bool));
		char* memChunk = NULL;
		if (!isNull)
		{
			memChunk = new char[size];
			eye.read(memChunk, size);
		}

		unsigned* addr32 = reinterpret_cast<unsigned*>(pMem);
		*addr32 = reinterpret_cast<unsigned>(memChunk);

		size = sizeof(unsigned);
	}
	else
	{
		eye.read(pMem, size);
	}

	return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeUserDefinedType(ISerializable* pMem, TypeNode* pType, bool isPtr, fstream& eye)
{
	int     size = 0;
	ISerializable* pObj = NULL;
	bool    isNull = false;

	if (isPtr)
	{
		unsigned* addr32;
		eye.read(reinterpret_cast<char*>(&isNull), sizeof(bool));
		addr32 = reinterpret_cast<unsigned*>(pMem);

		if (!isNull)
		{
			string  typeName;
			char    buffer[MaxTypeNameLength + 1];

			eye.read(buffer, MaxTypeNameLength + 1);
			typeName = buffer;
			pObj = static_cast<ISerializable*>(g_ObjectFactory.Create(typeName));

			PerformLateBinding(pObj, pType);

			auto objLayout = pObj->GetObjectLayout();
			Iterator* addresses = objLayout.GetIterator();
			unsigned* addr32;
			for (int memberIdx = 0; addresses->MoveNext(); ++memberIdx)
			{
				_ASSERTE(memberIdx < pType->Children.size());
				addr32 = reinterpret_cast<unsigned*>(addresses->Current());
				DeserializeType(reinterpret_cast<char*>(*addr32), pType->Children[memberIdx].Ptr32, eye);
			}
		}

		*addr32 = reinterpret_cast<unsigned>(pObj);
		size = sizeof(unsigned);
	}
	else
	{
		pObj = reinterpret_cast<ISerializable*>(pMem);

		auto objLayout = pObj->GetObjectLayout();
		Iterator* addresses = objLayout.GetIterator();
		unsigned* addr32;
		for (int memberIdx = 0; addresses->MoveNext(); ++memberIdx)
		{
			_ASSERTE(memberIdx < pType->Children.size());
			addr32 = reinterpret_cast<unsigned*>(addresses->Current());
			DeserializeType(reinterpret_cast<char*>(*addr32), pType->Children[memberIdx].Ptr32, eye);
		}

		size = objLayout.TypeSize();
	}

	return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeArray(char* pMem, TypeNode* pType, fstream& eye)
{
	int         size = 0;
	int         length = pType->Children[0].Val32;
	TypeNode*   type = pType->Children[1].Ptr32;
	bool        isNull = false;

	if (pType->Indirection)
	{
		unsigned* addr32 = reinterpret_cast<unsigned*>(pMem);
		*addr32 = NULL;

		eye.read(reinterpret_cast<char*>(&isNull), sizeof(bool));
		if (!isNull)
		{
			for (int i = 0; i < length; ++i)
			{
				pMem = reinterpret_cast<char*>(*addr32);
				DeserializeType(pMem, type, eye);
				addr32++;
			}
		}

		size = sizeof(unsigned)* length;
	}
	else
	{
		int s = 0;
		for (int i = 0; i < length; ++i)
		{
			s = DeserializeType(pMem, type, eye);
			pMem += s;
			size += s;
		}
	}

	return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeString(char* pMem, TypeNode* pType, fstream& eye)
{
	int     size = 0;
	int     length = 0;
	char*   buffer = NULL;
	string* str = NULL;
	bool    isNull = false;

	if (pType->Indirection)
	{
		unsigned* addr32 = reinterpret_cast<unsigned*>(pMem);
		eye.read(reinterpret_cast<char*>(&isNull), sizeof(bool));

		if (!isNull)
		{
			eye.read(reinterpret_cast<char*>(&length), sizeof(int));
			buffer = new char[length + 1];
			eye.read(reinterpret_cast<char*>(buffer), sizeof(char)* length);
			buffer[length] = 0;

			str = new string(buffer);
			addr32 = reinterpret_cast<unsigned*>(pMem);
		}

		*addr32 = reinterpret_cast<unsigned>(str);
		size = sizeof(unsigned);
	}
	else
	{
		eye.read(reinterpret_cast<char*>(&length), sizeof(int));
		buffer = new char[length + 1];
		eye.read(reinterpret_cast<char*>(buffer), sizeof(char)* length);
		buffer[length] = 0;

		str = reinterpret_cast<string*>(pMem);
		*str = buffer;
		size = sizeof(string);
	}

	if (buffer != NULL)
		delete[] buffer;

	return size;
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeContainerVector(ISerializable* pMem, TypeNode* pType, fstream& eye)
{
	// Pointer to vector is not supported
	_ASSERTE(pType->Indirection == false);

	auto objLayout = pMem->GetObjectLayout();
	IContainer* pContainer = dynamic_cast<IContainer*>(pMem);
	_ASSERTE(pContainer);
	int count;

	eye.read(reinterpret_cast<char*>(&count), sizeof(int));
	pContainer->Clear();

	char* tempStorage = pContainer->GetTemp();
	for (int i = 0; i < count; ++i)
	{
		DeserializeType(tempStorage, pType->TemplateArguments[0], eye);
		pContainer->AddTemp();
	}

	return objLayout.TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeContainerMap(ISerializable* pMem, TypeNode* pType, fstream& eye)
{
	// Pointer to map is not supported
	_ASSERTE(pType->Indirection == false);

	auto objLayout = pMem->GetObjectLayout();
	IContainer* pContainer = dynamic_cast<IContainer*>(pMem);
	_ASSERTE(pContainer);
	int count;

	eye.read(reinterpret_cast<char*>(&count), sizeof(int));
	pContainer->Clear();

	char* tempStorage = pContainer->GetTemp();
	for (int i = 0; i < count; ++i)
	{
		DeserializeType(tempStorage, pType->TemplateArguments[0], eye);
		pContainer->AddTemp();
	}

	return objLayout.TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializeContainerSet(ISerializable* pMem, TypeNode* pType, fstream& eye)
{
	// Pointer to set is not supported
	_ASSERTE(pType->Indirection == false);

	auto objLayout = pMem->GetObjectLayout();
	IContainer* pContainer = dynamic_cast<IContainer*>(pMem);
	_ASSERTE(pContainer);
	int count;

	eye.read(reinterpret_cast<char*>(&count), sizeof(int));
	pContainer->Clear();
	char* tempStorage0 = pContainer->GetTemp();

	for (int i = 0; i < count; ++i)
	{
		DeserializeType(tempStorage0, pType->TemplateArguments[0], eye);
		pContainer->AddTemp();
	}

	return objLayout.TypeSize();
}
//----------------------------------------------------------------------------------------------
int ObjectSerializer::DeserializePair(ISerializable* pObj, TypeNode* pType, fstream& eye)
{
	// Pointer to pair is not supported
	_ASSERTE(pType->Indirection == false);

	auto objLayout = pObj->GetObjectLayout();
	Iterator* addresses = objLayout.GetIterator();
	unsigned* addr32;
	for (int memberIdx = 0; addresses->MoveNext(); ++memberIdx)
	{
		addr32 = reinterpret_cast<unsigned*>(addresses->Current());
		DeserializeType(reinterpret_cast<char*>(*addr32), pType->TemplateArguments[memberIdx], eye);
	}

	return objLayout.TypeSize();
}

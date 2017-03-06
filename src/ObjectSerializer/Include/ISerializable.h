#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <typeinfo>
#include <stdarg.h>
#include <string>
#include <vector>
#include "ITraversable.h"

namespace Serialization
{
	template<class T>
	class VectorIterator : public Iterator
	{
		bool m_initialized;
		const std::vector<T>* m_pInnerVector;
		typename std::vector<T>::const_iterator m_current;

	public:
		VectorIterator(const std::vector<T>* p_vector) : m_pInnerVector(p_vector), m_initialized(false) {}

		char* Current()
		{
			return reinterpret_cast<char*>(const_cast<T*>(&*m_current));
		}

		bool MoveNext()
		{
			if (!m_initialized)
			{
				m_current = m_pInnerVector->begin();
			}

			if (m_current != m_pInnerVector->end())
			{
				if (!m_initialized)
				{
					m_initialized = true;
					return true;
				}
				else
				{
					++m_current;
					if (m_current == m_pInnerVector->end())
						return false;
					else
						return true;
				}
			}
			else
			{
				return false;
			}
		}

		void Reset()
		{
			m_initialized = false;
			m_current = m_pInnerVector->begin();
		}
	};

	class ObjectLayout : public ITraversable
	{
	public:
		template<typename TObject, typename... Args>
		ObjectLayout(const char* pTypeName, size_t typeSize, const TObject* pRootAddress, Args... childrenAddresses) :
			m_pTypeName(pTypeName),
			m_typeSize(typeSize),
			m_pItr(nullptr),
			m_pRootAddress((char*)pRootAddress),
			m_pCName(typeid(*pRootAddress).name())
		{
			AddMemberAddress(sizeof...(Args), childrenAddresses...);
		}

		template<typename TObject, typename... Args>
		ObjectLayout(Iterator* pItr, const char* pTypeName, size_t typeSize, const TObject* pRootAddress, Args... childrenAddresses) :
			m_pTypeName(pTypeName),
			m_typeSize(typeSize),
			m_pItr(pItr),
			m_pRootAddress((char*)pRootAddress),
			m_pCName(typeid(*pRootAddress).name())
		{
			AddMemberAddress(sizeof...(Args), childrenAddresses...);
		}

		template<typename TObject, typename... Args>
		ObjectLayout(ObjectLayout parentLayout, const char* pTypeName, size_t typeSize, const TObject* pRootAddress, Args... childrenAddresses) :
			m_pTypeName(pTypeName),
			m_typeSize(typeSize),
			m_pItr(nullptr),
			m_pRootAddress((char*)pRootAddress),
			m_childrenAdresses(parentLayout.ChildAddresses().begin(), parentLayout.ChildAddresses().end()),
			m_pCName(typeid(*pRootAddress).name())
		{
			AddMemberAddress(sizeof...(Args), childrenAddresses...);
		}

		char* RootAddress() { return m_pRootAddress; }
		const std::vector<char*>& ChildAddresses() { return m_childrenAdresses; }
		Iterator* GetIterator() const { return (m_pItr != nullptr ? m_pItr : new VectorIterator<char*>(&m_childrenAdresses)); }
		std::string TypeName()  const { return m_pTypeName; }
		int TypeSize()  const { return (int)m_typeSize; }
		std::string CName() const { return m_pCName; }

	private:
		void AddMemberAddress(size_t nAddresses, ...)
		{
			void* memberAddress;
			va_list argList;
			// The va_start macro is usually equivalent to:
			// pArgList = (void*) &p_memberAddress + sizeof (p_memberAddress) ;
			va_start(argList, nAddresses);
			++nAddresses;
			while (--nAddresses)
			{
				memberAddress = va_arg(argList, void*);
				m_childrenAdresses.push_back(reinterpret_cast<char*>(memberAddress));
			}
			va_end(argList);
		}

		const char* m_pCName;
		const char* m_pTypeName;
		size_t m_typeSize;
		char* m_pRootAddress;
		std::vector<char*> m_childrenAdresses;
		Iterator* m_pItr;
	};

	class ISerializable
	{
	public:
		virtual ~ISerializable() {}
		virtual ObjectLayout GetObjectLayout() const = 0;
	};

	extern size_t sm_lastSerializableObjID;
}

#define OBJECT_SERIALIZABLE(C, ...) \
	public: \
	Serialization::ObjectLayout GetObjectLayout() const { return Serialization::ObjectLayout(#C, sizeof(*this), this, __VA_ARGS__); }

#define OBJECT_SERIALIZABLE_P(C, P, ...) \
	public: \
	Serialization::ObjectLayout GetObjectLayout() const { return Serialization::ObjectLayout(P::GetObjectLayout(), #C, sizeof(*this), this, __VA_ARGS__); }

#define OBJECT_SERIALIZABLE_CONT(C, ...) \
	public: \
	Serialization::ObjectLayout GetObjectLayout() const { return Serialization::ObjectLayout(CreateIterator(), #C, sizeof(*this), this, __VA_ARGS__); }

#endif // SERIALIZABLE_H
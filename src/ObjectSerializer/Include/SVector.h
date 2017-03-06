#ifndef SVECTOR_H
#define SVECTOR_H

#include <vector>
#include "Container.h"
#include "ISerializable.h"

namespace Serialization
{
	template<class T>
	class SVector : public std::vector<T>, public IContainer
	{
		OBJECT_SERIALIZABLE_CONT(SVector);
	public:
		SVector() {}
		SVector(size_t p_initialSize, T p_initialValue) : vector(p_initialSize, p_initialValue) {}
		SVector(const std::vector<T>& p_other) : std::vector<T>(p_other) {}
		int             ContainerCount() const { return size(); }
		void            Clear()         { clear(); }
		char*           GetTemp()       { return reinterpret_cast<char*>(&m_temp); }
		void            AddTemp()       { push_back(m_temp); }
		Iterator*		CreateIterator() const { return new VectorIterator<T>(this); }

	private:
		T m_temp;
	};
	
}
#endif // SVECTOR_H

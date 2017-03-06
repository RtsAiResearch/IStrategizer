#ifndef SSET_H
#define SSET_H

#include <set>
#include "Container.h"
#include "ISerializable.h"

namespace Serialization
{
    template<class TKey>
    class SetIterator;

    template<class TKey>
    class SSet : public std::set<TKey>, public IContainer
    {
		OBJECT_SERIALIZABLE_CONT(SSet);
    public:
		SSet() {}
		SSet(const std::set<TKey>& p_other) : std::set<TKey>(p_other) {}
        int             ContainerCount() const { return size(); }
        void            Clear()         { clear(); }
        char*           GetTemp()       { return reinterpret_cast<char*>(&m_temp); }
        void            AddTemp()       { insert(m_temp); }
		Iterator*		CreateIterator() const { return new SetIterator<TKey>(this); }
	private:
        TKey    m_temp;
    };
    //----------------------------------------------------------------------------------------------
    template<class TKey>
    class SetIterator : public Iterator
    {
        bool m_initialized;
        const std::set<TKey>* m_set;  
        typename std::set<TKey>::const_iterator m_current;

    public:
        SetIterator(const std::set<TKey>* p_set) :  m_set(p_set), m_current(p_set->begin()), m_initialized(false) {}

        char* Current()
        {
            return reinterpret_cast<char*>(const_cast<TKey*>(&*m_current));
        }

        bool MoveNext()
        {
            if(m_current != m_set->end())
            {
                if(!m_initialized)
                {
                    m_initialized = true;
                    return true;
                }
                else
                {
                    ++m_current;
                    if(m_current == m_set->end())
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
            m_current = m_set->begin();
        }
    };

}
#endif // SSET_H

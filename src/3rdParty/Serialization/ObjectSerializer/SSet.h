#ifndef SSET_H
#define SSET_H

#include <set>

#ifndef CONTAINER_H
#include "Container.h"
#endif

namespace Serialization
{
    template<class TKey>
    class SetIterator;

    template<class TKey>
    class SSet : public Container, public std::set<TKey>
    {
        TKey    m_temp;
    public:
        Iterator*       GetIterator()   { return new SetIterator<TKey>(this); }
        std::string     TypeName() const      { return "SSet"; }
        int             TypeSize() const      { return sizeof(SSet<TKey>); }
        ISerializable*  Prototype() const     { return new SSet<TKey>; }
        int             ContainerCount(){ return size(); }
        void            Clear()         { clear(); }
        char*           GetTemp()       { return reinterpret_cast<char*>(&m_temp); }
        void            AddTemp()       { insert(m_temp); }
    };
    //----------------------------------------------------------------------------------------------
    template<class TKey>
    class SetIterator : public Iterator
    {
        bool                            m_initialized;
        std::set<TKey>*                      m_set;  
        typename std::set<TKey>::iterator    m_current;

    public:
        SetIterator(std::set<TKey>* p_set) :  m_set(p_set), m_current(p_set->begin()), m_initialized(false) {}

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

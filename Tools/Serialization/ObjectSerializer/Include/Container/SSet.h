#ifndef SSET_H
#define SSET_H

#include <set>
using namespace std;

#ifndef CONTAINER_H
#include "Container.h"
#endif

namespace Serialization
{
    template<class TKey>
    class SetIterator;

    template<class TKey>
    class SSet : public set<TKey>, public Container
    {
        TKey    m_temp;
    public:
        Iterator*       GetIterator()   { return new SetIterator<TKey>(this); }
        string          TypeName()      { return "SSet"; }
        int             TypeSize()      { return sizeof(SSet<TKey>); }
        Serializable*   Prototype()     { return new SSet<TKey>; }
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
        set<TKey>*                      m_set;  
        typename set<TKey>::iterator    m_current;

    public:
        SetIterator(set<TKey>* p_set) :  m_set(p_set), m_current(p_set->begin()), m_initialized(false) {}

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
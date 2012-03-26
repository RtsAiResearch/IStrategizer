#ifndef SMAP_H
#define SMAP_H

#include <map>
using namespace std;

#ifndef CONTAINER_H
#include "Container.h"
#endif

#ifndef SPAIR_H
	#include "SPair.h"
#endif

namespace Serialization
{
    template<class TKey, class TValue>
    class MapIterator;

    template<class TKey, class TValue>
    class SMap : public map<TKey, TValue>, public Container
    {
        SPair<TKey, TValue> m_temp;
    public:
        Iterator*       GetIterator()   { return new MapIterator<TKey, TValue>(this); }
        string          TypeName()      { return "SMap"; }
        int             TypeSize()      { return sizeof(SMap<TKey, TValue>); }
        Serializable*   Prototype()     { return new SMap<TKey, TValue>; }
        int             ContainerCount(){ return size(); }
        void            Clear()         { clear(); }
        char*           GetTemp()       { return reinterpret_cast<char*>(&m_temp); }
        void            AddTemp()       { insert(m_temp); }
    };
    //----------------------------------------------------------------------------------------------
    template<class TKey, class TValue>
    class MapIterator : public Iterator
    {
        bool                                    m_initialized;
        map<TKey, TValue>*                      m_map;  
        typename map<TKey, TValue>::iterator    m_current;
        SPair<TKey, TValue>                      m_pair;

    public:
        MapIterator(map<TKey, TValue>* p_map) :  m_map(p_map), m_current(p_map->begin()), m_initialized(false) {}

        virtual char* Current()
        {
            return reinterpret_cast<char*>(&m_pair);
        }

        bool MoveNext()
        {
            if(!m_initialized)
            {
                m_current = m_map->begin();
            }

            if(m_current != m_map->end())
            {
                if(!m_initialized)
                {
                    m_initialized = true;
                    m_pair.first   = m_current->first;
                    m_pair.second  = m_current->second;
                    return true;
                }
                else
                {
                    ++m_current;
                    if(m_current == m_map->end())
                        return false;
                    else
                    {
                        m_pair.first   = m_current->first;
                        m_pair.second  = m_current->second;
                        return true;
                    }
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
            m_current = m_map->begin();
        }
    };
}
#endif // SMAP_H

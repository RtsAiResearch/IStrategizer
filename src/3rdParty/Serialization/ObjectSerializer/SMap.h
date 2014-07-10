#ifndef SMAP_H
#define SMAP_H

#include <map>
#include "Container.h"
#include "SPair.h"

namespace Serialization
{
    template<class TKey, class TValue>
    class MapIterator;

    template<class TKey, class TValue>
    class SMap : public std::map<TKey, TValue>, public IContainer
    {
		OBJECT_SERIALIZABLE_CONT(SMap);
    public:
		SMap() {}
		SMap(const std::map<TKey, TValue>& p_other) : std::map<TKey, TValue>(p_other) {}
        int             ContainerCount() const { return size(); }
        void            Clear()         { clear(); }
        char*           GetTemp()       { return reinterpret_cast<char*>(&m_temp); }
        void            AddTemp()       { insert(m_temp); }
		Iterator*		CreateIterator() const { return new MapIterator<TKey, TValue>(this); }

        void Keys(std::vector<TKey>& keys) const 
        {
            keys.resize(size());

            int i = 0;
            for(std::map<TKey, TValue>::const_iterator itr = begin();
                itr != end();
                ++itr, ++i)
            {
                keys[i] = itr->first;
            }
        }

        void Values(std::vector<TValue>& values) const
        {
            values.resize(size());

            int i = 0;
            for(std::map<TKey, TValue>::const_iterator itr = begin();
                itr != end();
                ++itr, ++i)
            {
                values[i] = itr->second;
            }
        }

        bool TryGet(const TKey& key, TValue& value) const
        {
            if(count(key) > 0)
            {
                value = at(key);
                return true;
            }

            return false;
        }

        bool Contains(const TKey& key) const { return count(key) > 0; }

	private:
		SPair<TKey, TValue> m_temp;
    };
    //----------------------------------------------------------------------------------------------
    template<class TKey, class TValue>
    class MapIterator : public Iterator
    {
        bool m_initialized;
        const std::map<TKey, TValue>* m_map;  
        typename std::map<TKey, TValue>::const_iterator m_current;
        SPair<TKey, TValue> m_pair;

    public:
        MapIterator(const std::map<TKey, TValue>* p_map) :  m_map(p_map), m_current(p_map->cbegin()), m_initialized(false) {}

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

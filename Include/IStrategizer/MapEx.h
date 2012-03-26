#ifndef MAPEX_H
#define MAPEX_H

#include <map>
#include <vector>

namespace DataStructure
{
    template<class TKey, class TValue>
    class MapEx : public std::map<TKey, TValue>
    {
    public:
        void Keys(std::vector<TKey>& p_keys) const 
        {
            p_keys.resize(size());

            int i = 0;
            for(DynamicMap<TKey, TValue>::const_iterator itr = begin();
                itr != end();
                ++itr, ++i)
            {
                p_keys[i] = itr->first;
            }
        }
        //----------------------------------------------------------------------------------------------
        void Values(std::vector<TValue>& p_values) const
        {
            p_values.resize(size());

            int i = 0;
            for(DynamicMap<TKey, TValue>::const_iterator itr = begin();
                itr != end();
                ++itr, ++i)
            {
                p_values[i] = itr->second;
            }
        }
        //----------------------------------------------------------------------------------------------
        bool TryGet(const TKey& p_key, TValue& p_value) const
        {
            if(Contains(p_key))
            {
                p_value = this->operator [](p_key);
                return true;
            }

            return false;
        }
        //----------------------------------------------------------------------------------------------
        bool Contains(const TKey& p_key) const
        {
            return find(p_key) != end();
        }
    };
}

#endif // MAPEX_H

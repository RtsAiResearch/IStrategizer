#ifndef DYNAMICSET_H
#define DYNAMICSET_H

#include <set>

#ifndef DYNAMICCOMPONENT_H
#include "DynamicComponent.h"
#endif

namespace IStrategizer
{
    template<class TKey>
    class DynamicSet : public std::set<TKey>, public DynamicComponent
    {
    public:
        void Keys(std::vector<TKey>& p_keys) const 
        {
            p_keys.resize(size());

            int i = 0;
            for(DynamicSet<TKey>::const_iterator itr = begin();
                itr != end();
                ++itr, ++i)
            {
                p_keys[i] = *itr;
            }
        }
        //----------------------------------------------------------------------------------------------
        bool Contains(const TKey& p_key) const
        {
            return find(p_key) != end();
        }
    };
}


#endif // DYNAMICSET_H
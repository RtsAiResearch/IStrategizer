#ifndef SPAIR_H
#define SPAIR_H

#include <map>
#include "ISerializable.h"

namespace Serialization
{
    template<class TKey, class TValue>
    class SPair : public std::pair<TKey, TValue>, public ISerializable 
    {
        OBJECT_SERIALIZABLE(SPair, &first, &second);

    public:
        SPair() : pair(TKey(), TValue()) {}
        SPair(TKey p_first, TValue p_second) : pair(p_first, p_second) {}
    };

    template<class TKey, class TValue>
    SPair<TKey, TValue> inline MakePair(TKey p_first, TValue p_second) { return SPair<TKey, TValue>(p_first, p_second); }
    //----------------------------------------------------------------------------------------------
}
#endif // SPAIR_H

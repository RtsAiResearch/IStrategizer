#ifndef SPAIR_H
#define SPAIR_H

#include <map>

#ifndef USEROBJECT_H
    #include "UserObject.h"
#endif

namespace Serialization
{
    template<class TKey, class TValue>
    class SPair : public UserObject, public std::pair<TKey, TValue>
    {
    public:
                        SPair() : pair(TKey(), TValue()) {}
                        SPair(TKey p_first, TValue p_second) : pair(p_first, p_second) {}
        std::string     TypeName() const      { return "SPair"; }
        int             TypeSize() const      { return sizeof(SPair<TKey, TValue>); }
        ISerializable*  Prototype() const     { return new SPair<TKey, TValue>; }
    protected:

        void InitializeAddressesAux()
        {
            AddMemberAddress(2,
                &first,
                &second);
        }

    };
    template<class TKey, class TValue>
    SPair<TKey, TValue> inline MakePair(TKey p_first, TValue p_second) { return SPair<TKey, TValue>(p_first, p_second); }
    //----------------------------------------------------------------------------------------------
}
#endif // SPAIR_H

#ifndef SPAIR_H
#define SPAIR_H

#include <map>
using namespace std;

#ifndef USEROBJECT_H
	#include "UserObject.h"
#endif

namespace Serialization
{
    template<class TKey, class TValue>
    class SPair : public pair<TKey, TValue>, public UserObject
    {
    public:
                        SPair() : pair(TKey(), TValue()) {}
                        SPair(TKey p_first, TValue p_second) : pair(p_first, p_second) {}
        string          TypeName()      { return "SPair"; }
        int             TypeSize()      { return sizeof(SPair<TKey, TValue>); }
        Serializable*   Prototype()     { return new SPair<TKey, TValue>; }
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

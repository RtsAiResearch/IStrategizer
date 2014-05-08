#ifndef COLLECTION_H
#define COLLECTION_H

namespace IStrategizer
{
    const int MaxItems    = 512;

    template<class TPKey>
    class Collection
    {
    public:
        TPKey   Items[MaxItems];
        int     Count;
        //----------------------------------------------------------------------------------------------
        Collection() : Count(0)
        {
            memset(Items, TPKey(), MaxItems * sizeof(TPKey));
        }
        //----------------------------------------------------------------------------------------------
        void Add(TPKey p_pkey)
        {
            _ASSERTE(Count + 1 < MaxItems);
            Items[Count] = p_pkey;
            ++Count;
        }
    };
}

#endif // COLLECTION_H

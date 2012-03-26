#ifndef COLLECTION_H
#define COLLECTION_H

namespace DataStructure
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
            assert(Count + 1 < MaxItems);
            Items[Count] = p_pkey;
            ++Count;
        }
    };
}

#endif // COLLECTION_H
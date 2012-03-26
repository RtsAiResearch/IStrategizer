#ifndef MULTIINDEX_H
#define MULTIINDEX_H

#include <cassert>
#include <cstdlib>

namespace DataStructure
{
    const int MaxIndices    = 16;

    template<class TSKey>
    class MultiIndex
    {
    public:
        TSKey   Components[MaxIndices];
        int     Count;

        //----------------------------------------------------------------------------------------------
        MultiIndex(int p_count, ...) : Count(p_count)
        {
            assert(p_count > 0);
            int     i = -1;
            va_list argList;

            va_start (argList, p_count);
            ++p_count;
            while(--p_count)
            {
                Components[++i] = va_arg(argList, TSKey);
            }
            va_end (argList) ;
        }
        //----------------------------------------------------------------------------------------------
        MultiIndex() : Count(0)
        {
            memset(Components, TSKey(), MaxIndices * sizeof(TSKey));
        }
    };
}
#endif // MULTIINDEX_H
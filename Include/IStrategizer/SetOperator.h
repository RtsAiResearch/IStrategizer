#ifndef SETOPERATOR_H
#define SETOPERATOR_H

#include <set>
#include <cassert>
using namespace std;

#ifndef COLLECTION_H
#include "Collection.h"
#endif

namespace DataStructure
{
    const int MaxSets = 16;

    template<class T>
    class SetOperator
    {
        typename set<T>::iterator   m_indices[MaxSets];
        int                         m_setsCount;
        set<T>**                    m_sets;

    public:
        void Intersect(set<T>** p_sets, int p_setsCount, Collection<T>& p_result)
        {
            assert(p_setsCount > 0);

            m_setsCount     = p_setsCount;
            m_sets          = p_sets;

            int maxValIdx = 0;

            InitializeIndices();
            while(1)
            {
                if(MatchCurrent(maxValIdx))
                {
                    p_result.Add(*m_indices[0]);
                    if(!MoveNext())
                        break;
                }
                else
                {
                    if(!MoveNext(maxValIdx))
                        break;
                }
            }
        }
    protected:
        //----------------------------------------------------------------------------------------------
        bool MoveNext()
        {
            bool allOk = true;
            for(int i = 0; i < m_setsCount; ++i)
            {
                allOk &= AdvanceIndex(m_indices[i], m_sets[i]);
            }

            return allOk;
        }
        //----------------------------------------------------------------------------------------------
        bool MatchCurrent(int& p_maxValIdx)
        {
            bool    matchAll = true;

            p_maxValIdx = 0;
            for(int i = 1; i < m_setsCount; ++i)
            {
                if(matchAll)
                    matchAll &= *m_indices[i] == *m_indices[0];

                if(!matchAll && *m_indices[i] > *m_indices[p_maxValIdx])
                    p_maxValIdx = i;
            }

            return matchAll;
        }
        //----------------------------------------------------------------------------------------------
        void InitializeIndices()
        {
            for(int i = 0; i < m_setsCount; ++i)
                m_indices[i] = m_sets[i]->begin();
        }
        //----------------------------------------------------------------------------------------------
        bool MoveNext(int p_targetSetIdx)
        {
            bool allOk = true;
            for(int i = 0; i < m_setsCount; ++i)
            {
                if(i == p_targetSetIdx)
                    continue;

                allOk &= AdvanceIndex(m_indices[i], m_sets[i], m_indices[p_targetSetIdx]);
            }

            return allOk;
        }

        //----------------------------------------------------------------------------------------------
        bool AdvanceIndex(typename set<T>::iterator& p_currentItr, const set<T>* p_set, typename set<T>::iterator& p_targetValItr)
        {
            while(p_currentItr != p_set->end() && *p_currentItr < *p_targetValItr)
                ++p_currentItr;

            return p_currentItr != p_set->end();
        }
        //----------------------------------------------------------------------------------------------
        bool AdvanceIndex(typename set<T>::iterator& p_currentItr, const set<T>* p_set)
        {
            if(p_currentItr != p_set->end())
                ++p_currentItr;

            return p_currentItr != p_set->end();
        }
        //----------------------------------------------------------------------------------------------
    };
}
#endif // SETOPERATOR_H
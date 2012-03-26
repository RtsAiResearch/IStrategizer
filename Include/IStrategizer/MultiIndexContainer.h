#ifndef MULTIINDEXCONTAINER_H
#define MULTIINDEXCONTAINER_H

#include <set>
#include <hash_map>
#include <cassert>
#include <cstdarg>
#include <ostream>
using namespace std;
using namespace stdext;

#ifndef MULTIINDEX_H
#include "MultiIndex.h"
#endif

#ifndef COLLECTION_H
#include "Collection.h"
#endif

#ifndef SETOPERATOR_H
#include "SetOperator.h"
#endif

namespace DataStructure
{
    template<class TPKey, class TSKey, class TValue>
    class MultiIndexContainer
    {
    public:
        class Record
        {
        public:
            TPKey               PKey;
            MultiIndex<TSKey>   SKeys;
            TValue              UnmanagedData;
        };

        typedef set<TPKey>                  Bucket;
        typedef hash_map<TSKey, Bucket*>    SKeyIndexTable;
        typedef hash_map<TPKey, Record*>    PKeyIndexTable;

        SetOperator<TPKey>  m_setOperator;
        TSKey               m_nullSKey;
        SKeyIndexTable      m_sIndices[MaxIndices];
        int                 m_sIndexCount;
        PKeyIndexTable      m_pIndices;

        //----------------------------------------------------------------------------------------------
        MultiIndexContainer(int p_sIndexCount, TSKey p_nullSKey) : m_sIndexCount(p_sIndexCount), m_nullSKey(p_nullSKey)
        {

        }
        //----------------------------------------------------------------------------------------------
        void Add(TPKey p_pkey, MultiIndex<TSKey> p_indices, const TValue& p_val)
        {
            assert(m_pIndices.find(p_pkey) == m_pIndices.end());

            Record* record          = GetOrCreateRecord(p_pkey);
            record->SKeys           = p_indices;
            record->UnmanagedData   = p_val;

            for(int i = 0; i < p_indices.Count; ++i)
            {
                // null value
                if(p_indices.Components[i] == m_nullSKey)
                    continue;
                else
                {
                    Bucket* b = GetOrCreateBucket(i, p_indices.Components[i]);
                    b->insert(p_pkey); 
                }
            }
        }
        //----------------------------------------------------------------------------------------------
        void Delete(TPKey p_pkey)
        {
            Record* record = TryGetRecord(p_pkey);
            assert(record != NULL);

            MultiIndex<TSKey>& sIndices = record->SKeys;
            for(int i = 0; i < sIndices.Count; ++i)
            {
                SKeyIndexTable& sIndexTable = m_sIndices[i];
                TSKey skey = sIndices.Components[i];

                if(skey == m_nullSKey)
                    continue;

                sIndexTable[skey]->erase(p_pkey);
            }

            FreeRecord(p_pkey);
        }
        //----------------------------------------------------------------------------------------------
        void Select(MultiIndex<TSKey> p_sIndicies, Collection<TPKey>& p_result)
        {
            Bucket* candidateBuckets[MaxIndices];
            int     bucketsCount = 0;

            for(int i = 0; i < p_sIndicies.Count; ++i)
            {
                SKeyIndexTable& sIndexTable = m_sIndices[i];
                TSKey skey = p_sIndicies.Components[i];

                if(skey == m_nullSKey)
                    continue;

                SKeyIndexTable::iterator where = sIndexTable.find(skey);

                // trying to select on a non existing SKey, no hope for select to succeed
                if(where == sIndexTable.end())
                    return;
                else
                    candidateBuckets[bucketsCount++] = where->second;
            }

            m_setOperator.Intersect(candidateBuckets, bucketsCount, p_result);
        }
        //----------------------------------------------------------------------------------------------
        void Select(TPKey p_pkey, TValue& p_result)
        {
            Record* record = TryGetRecord(p_pkey);
            p_result = record->UnmanagedData;
        }
        //----------------------------------------------------------------------------------------------
        void Visualize(ostream& p_output)
        {
            p_output << endl << "Displaying MultiIndexContainer: number of indices = " << m_sIndexCount << " index" << endl;
            for(int i = 0; i < m_sIndexCount; ++i)
            {
                SKeyIndexTable& idxTable = m_sIndices[i];

                p_output << "-----------------" << endl;
                p_output << "- Index Table #" << i << endl << "-----------------" << endl;
                for(SKeyIndexTable::iterator indexItr = idxTable.begin();
                    indexItr != idxTable.end();
                    ++indexItr)
                {
                    p_output << "\tIndex [" << indexItr->first << "]" << "\tBucket [";
                    Bucket* b = indexItr->second;

                    if(b == NULL)
                        p_output << "\t\tNull";
                    else
                    {
                        for(Bucket::iterator bucketItr = b->begin();
                            bucketItr != b->end();
                            ++bucketItr)
                        {
                            p_output << *bucketItr << ", ";
                        }
                    }
                    p_output << "Null]" << endl;
                }
            }

        }
        //----------------------------------------------------------------------------------------------
        virtual ~MultiIndexContainer()
        {
            for(int i = 0; i < m_sIndexCount; ++i)
            {
                FreeBuckets(i);
            }

            FreeRecords();
        }
        //----------------------------------------------------------------------------------------------
    protected:

        //----------------------------------------------------------------------------------------------
        Record* GetOrCreateRecord(const TPKey& p_pkey)
        {
            PKeyIndexTable::iterator itr = m_pIndices.find(p_pkey);

            if(itr == m_pIndices.end())
            {
                Record* record = new Record;
                m_pIndices[p_pkey] = record;

                return record;
            }
            else
                return itr->second;

        }
        //----------------------------------------------------------------------------------------------
        Record* TryGetRecord(const TPKey& p_pkey)
        {
            PKeyIndexTable::iterator itr = m_pIndices.find(p_pkey);

            if(itr == m_pIndices.end())
                return NULL;
            else
                return itr->second;
        }
        //----------------------------------------------------------------------------------------------
        Bucket* GetOrCreateBucket(int p_sIndexTable, const TSKey& p_indexVal)
        {
            SKeyIndexTable& sIndextTable = m_sIndices[p_sIndexTable];
            if(sIndextTable.find(p_indexVal) == sIndextTable.end())
            {
                sIndextTable[p_indexVal] = new Bucket();
            }

            return sIndextTable[p_indexVal];
        }
        //----------------------------------------------------------------------------------------------
        void FreeBuckets(int p_sIndexTable)
        {
            for(SKeyIndexTable::iterator itr = m_sIndices[p_sIndexTable].begin();
                itr != m_sIndices[p_sIndexTable].end();
                ++itr)
            {
                Bucket*& b = itr->second;
                if(b != NULL)
                    delete b;
            }
        }
        //----------------------------------------------------------------------------------------------
        void FreeRecords()
        {
            for(PKeyIndexTable::iterator itr = m_pIndices.begin();
                itr != m_pIndices.end();
                ++itr)
            {
                Record*& r = itr->second;
                if(r != NULL)
                    delete r;
            }

        }
        //----------------------------------------------------------------------------------------------
        void FreeRecord(const TPKey& p_pkey)
        {
            Record* record = m_pIndices[p_pkey];
            assert(record != NULL);
            delete record;
            m_pIndices.erase(p_pkey);
        }
    };
}

#endif // MULTIINDEXCONTAINER_H
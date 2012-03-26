#ifndef SVECTOR_H
#define SVECTOR_H

#include <vector>
using namespace std;

#ifndef CONTAINER_H
#include "Container.h"
#endif

namespace Serialization
{
    template<class T>
    class VectorIterator;

    template<class T>
    class SVector : public vector<T>, public Container
    {
        T m_temp;
    public:
                        SVector() {}
                        SVector(size_t p_initialSize, T p_initialValue) : vector(p_initialSize, p_initialValue) {}
                        SVector(const vector<T>& p_other) { this->clear(); insert(this->begin(), p_other.begin(), p_other.end()); }
        Iterator*       GetIterator()   { return new VectorIterator<T>(this); }
        string          TypeName()      { return "SVector"; }
        int             TypeSize()      { return sizeof(SVector<T>); }
        Serializable*   Prototype()     { return new SVector<T>; }
        int             ContainerCount(){ return size(); }
        void            Clear()         { clear(); }
        char*           GetTemp()       { return reinterpret_cast<char*>(&m_temp); }
        void            AddTemp()       { push_back(m_temp); }
    };
    //----------------------------------------------------------------------------------------------
    template<class T>
    class VectorIterator : public Iterator
    {
        bool                            m_initialized;
        SVector<T>*                      m_vector;  
        typename vector<T>::iterator    m_current;

    public:
        VectorIterator(SVector<T>* p_vector) :  m_vector(p_vector), m_initialized(false) {}
        
        char* Current() 
        {
            return reinterpret_cast<char*>(&*m_current);
        }

        bool MoveNext()
        {
            if(!m_initialized)
            {
                m_current = m_vector->begin();
            }

            if(m_current != m_vector->end())
            {
                if(!m_initialized)
                {
                    m_initialized = true;
                    return true;
                }
                else
                {
                    ++m_current;
                    if(m_current == m_vector->end())
                        return false;
                    else
                        return true;
                }
            }
            else
            {
                return false;
            }
        }

        void Reset()
        {
            m_initialized = false;
            m_current = m_vector->begin();
        }
    };
}
#endif // SVECTOR_H

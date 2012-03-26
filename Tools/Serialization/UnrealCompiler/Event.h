#ifndef EVENT_H
#define EVENT_H

#include "Delegate.h"
#include <vector>
#include <cassert>
using namespace std;

template<class TSender>
class Event
{
    vector< BaseDelegate<TSender>* > m_observers;
    void Register( const BaseDelegate<TSender>* p_handler);
    void Unregister( const BaseDelegate<TSender>* p_handler);
public:
    void operator += ( const BaseDelegate<TSender>* p_handler);
    void operator -= ( const BaseDelegate<TSender>* p_handler);
    void operator () ( const TSender* p_sender, void* p_parameter);
    void Call( const TSender* p_sender, void* p_parameter);
    ~Event();
};

template<class TSender>
inline void Event<TSender>::operator += (const BaseDelegate<TSender>* p_handler)
{
    Register(p_handler);
}

template<class TSender>
inline void Event<TSender>::operator -= (const BaseDelegate<TSender>* p_handler)
{
    Unregister(p_handler);
}

template<class TSender>
inline void Event<TSender>::operator ()(const TSender* p_sender, void* p_parameter)
{
    Call(p_sender, p_parameter);
}

template<class TSender>
inline void Event<TSender>::Register(const BaseDelegate<TSender>* p_handler)
{
    for(vector< BaseDelegate<TSender>* >::iterator itr = m_observers.begin();
       itr != m_observers.end();
       itr++)
    {
        if((*itr)->Equals(p_handler))
            return;
    }

    m_observers.push_back(const_cast<BaseDelegate<TSender>*>(p_handler));
}

template<class TSender>
inline void Event<TSender>::Unregister(const BaseDelegate<TSender>* p_handler)
{
    vector< BaseDelegate<TSender>* >::iterator where;
    for(vector< BaseDelegate<TSender>* >::iterator itr = m_observers.begin();
        itr != m_observers.end();
        itr++)
    {
        if((*itr)->Equals(p_handler))
        {
            where = itr;
            break;
        }
    }

    m_observers.erase(where);
}


template<class TSender>
inline void Event<TSender>::Call(const TSender* p_sender, void* p_parameter)
{
    for(vector< BaseDelegate<TSender>* >::iterator itr = m_observers.begin();
        itr != m_observers.end();
        itr++)
    {
        (*itr)->Call(p_sender, p_parameter);
    }
}

template<class TSender>
Event<TSender>::~Event()
{
    for(vector< BaseDelegate<TSender>* >::iterator itr = m_observers.begin();
        itr != m_observers.end();
        itr++)
    {
        assert(*itr != NULL);
        delete (*itr);
    }
}
#endif // EVENT_H
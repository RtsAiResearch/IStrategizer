#ifndef DELEGATE_H
#define DELEGATE_H

#include <cassert>

template<class TSender>
class BaseDelegate
{
public:
    virtual bool Equals( const BaseDelegate<TSender>* p_other) = 0;
    virtual void operator()( const TSender* p_sender, void* p_parameter) = 0;
    virtual void Call( const TSender* p_sender, void* p_parameter) = 0;
};

template<class TReciever, class TSender>
class Delegate : public BaseDelegate<TSender>
{
private:
    typedef void (TReciever::*PTF)(const TSender*, void* p_parameter);
    PTF         m_ptr2Func;
    TReciever*  m_ptr2Object;

public:
    Delegate(TReciever* p_ptr2Object, PTF p_ptr2Func)
    {
        m_ptr2Func      = p_ptr2Func;
        m_ptr2Object    = p_ptr2Object;
    }

    bool Equals(const BaseDelegate<TSender>* p_other)
    {
        const Delegate<TReciever, TSender>* other;

        other = static_cast<const Delegate<TReciever, TSender>*>(p_other);
        assert(other != NULL);

        return other->m_ptr2Object == m_ptr2Object && other->m_ptr2Func == m_ptr2Func;
    }

    virtual void operator()(const TSender* p_sender, void* p_parameter)
    {
        (m_ptr2Object->*m_ptr2Func)(p_sender, p_parameter);
    }

    virtual void Call(const TSender* p_sender, void* p_parameter)
    {
        (m_ptr2Object->*m_ptr2Func)(p_sender, p_parameter);
    }
};

#endif // DELEGATE_H
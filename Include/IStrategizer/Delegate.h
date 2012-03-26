#ifndef DELEGATE_H
#define DELEGATE_H

#include <cassert>

class BaseDelegate
{
public:
    virtual bool Equals( const BaseDelegate* p_other) = 0;
    virtual void operator()() = 0;
    virtual void Call() = 0;
};

template<class TReciever>
class Delegate : public BaseDelegate
{
private:
    typedef void (TReciever::*PTF)();
    PTF         m_ptr2Func;
    TReciever*  m_ptr2Object;

public:
    Delegate(TReciever* p_ptr2Object, PTF p_ptr2Func)
    {
        m_ptr2Func      = p_ptr2Func;
        m_ptr2Object    = p_ptr2Object;
    }

    bool Equals(const BaseDelegate* p_other)
    {
        const Delegate<TReciever>* other;

        other = static_cast<const Delegate<TReciever>*>(p_other);

        assert(other != NULL);
        assert(m_ptr2Object != NULL);

        return other->m_ptr2Object == m_ptr2Object && other->m_ptr2Func == m_ptr2Func;
    }

    virtual void operator()()
    {
        assert(m_ptr2Object != NULL);
        (m_ptr2Object->*m_ptr2Func)();
    }

    virtual void Call()
    {
        assert(m_ptr2Object != NULL);
        (m_ptr2Object->*m_ptr2Func)();
    }
};

#endif // DELEGATE_H
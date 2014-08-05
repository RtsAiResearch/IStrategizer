#include "Variant.h"

int Variant::ToInt() const 
{
    assert(m_type == Integer);
    assert(m_value != NULL);
    return *reinterpret_cast<int*>(m_value); 
}

string Variant::ToString() const 
{
    assert(m_type == String);
    assert(m_value != NULL);
    return *reinterpret_cast<string*>(m_value); 
}

void Variant::FromInt(int p_rhs)
{
    if(m_value == NULL)
    {
        m_value = new int(p_rhs);
        m_type  = Integer;
    }
    else
    {
        switch(m_type)
        {
        case Integer:
            *reinterpret_cast<int*>(m_value) = p_rhs;
            break;

        case String:
            Clean();
            m_value = new int(p_rhs);
            m_type  = Integer;
            break;

        case Undefined:
            assert(false);
        }
    }
}

void Variant::FromString(const string& p_rhs)
{
    if(m_value == NULL)
    {
        m_value = new string(p_rhs);
        m_type  = String;
    }
    else
    {
        switch(m_type)
        {
        case Integer:
            Clean();
            m_value = new string(p_rhs);
            m_type  = String;

            break;
        case String:
            *reinterpret_cast<string*>(m_value) = p_rhs;
            break;

        case Undefined:
            assert(false);
        }
    }
}

void Variant::Clean()
{
    if(m_value != NULL)
        delete m_value;

    m_value = NULL;
}

Variant::~Variant()
{
    Clean();
}


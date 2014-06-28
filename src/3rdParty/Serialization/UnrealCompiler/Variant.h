#ifndef VARIANT_H
#define VARIANT_H

#include <string>
using namespace std;
#include <cassert>

enum ValueType
{
    Undefined,
    Integer,
    String
};

class Variant
{
    void*       m_value;
    ValueType   m_type;

    int         ToInt() const;
    string      ToString() const;
    void        FromInt(int p_rhs);
    void        FromString(const string& p_rhs);
    void        Clean();

public:
    Variant() : m_value(NULL), m_type(Undefined) {}
    Variant(int p_value) : m_value(new int(p_value)) { m_type = Integer; }
    Variant(string& p_value) : m_value(new string(p_value)) { m_type = String; }

    Variant& operator = (int p_rhs) { assert(m_type == Integer); }
    Variant& operator = (const string& p_rhs);

    ValueType   Type() const { return m_type; }
    ~Variant();
};

#endif // VARIANT_H

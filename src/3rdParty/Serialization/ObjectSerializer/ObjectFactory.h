#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

#include <string>
#include <map>
using namespace std;
using namespace Serialization;

typedef map<string, UserObject*> ObjectTable;

namespace Serialization
{
    class ObjectFactory
    {
        map<string, string> m_cNameToFullNameTable;
        ObjectTable         m_prototypes;

    public:
        UserObject*     GetObject(const string& p_typeName);
        const string&   FromCName(const string& p_cName);
        UserObject*     AddPrototype(UserObject* p_prototype);
        UserObject*     AddPrototype(UserObject* p_prototype, char* p_fullName);
        ObjectTable&    GetObjectTable() { return m_prototypes; }
        static ObjectFactory& Instance() { static ObjectFactory instance; return instance; }
    };

#define g_ObjectFactory ObjectFactory::Instance()
#define DECL_SERIALIZABLE(C) \
    static Serialization::UserObject* g_p##C##PrototypeFactory_Internal = g_ObjectFactory.AddPrototype(new C);

}
#endif // OBJECTFACTORY_H_H

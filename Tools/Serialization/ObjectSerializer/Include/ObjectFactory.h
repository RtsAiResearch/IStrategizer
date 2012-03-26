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
        void            AddPrototype(UserObject* p_prototype);
        void            AddPrototype(UserObject* p_prototype, char* p_fullName);
        ObjectTable&    GetObjectTable() { return m_prototypes; }
        static ObjectFactory& Instance() { static ObjectFactory instance; return instance; }
#define g_ObjectFactory ObjectFactory::Instance()
    };
}
#endif // OBJECTFACTORY_H_H
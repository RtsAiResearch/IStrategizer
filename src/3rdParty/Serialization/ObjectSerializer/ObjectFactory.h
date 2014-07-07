#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

#include <string>
#include <map>
#include <memory>
using namespace std;
using namespace Serialization;

typedef std::map<std::string, Serialization::UserObject*> ObjectTable;

namespace Serialization
{
    typedef Serialization::UserObject* (*PfnUserObjectFactory)();
    typedef std::map<std::string, PfnUserObjectFactory> ObjectFactoryMap;

    template<class T>
    UserObject* UserObjectFactory() { return new T; }

    class ObjectFactory
    {
        map<string, string> m_cNameToFullNameTable;
        ObjectFactoryMap m_factories;

    public:
        UserObject* GetObject(const string& p_typeName);
        const string& FromCName(const string& p_cName);
        UserObject* Create(const string& p_typeName);

        template<class T>
        PfnUserObjectFactory AddPrototype(const char* pNameOverride = nullptr)
        {
            std::shared_ptr<T> pPrototype(new T);
            _ASSERTE(pPrototype != NULL);
            string cname = pPrototype->CName();

            if (nullptr != pNameOverride)
            {
                m_cNameToFullNameTable[cname] = pNameOverride;
                m_factories[pNameOverride] = &UserObjectFactory<T>;
            }
            else
            {
                string tname = pPrototype->TypeName();
                m_cNameToFullNameTable[cname] = tname;
                m_factories[tname] = &UserObjectFactory<T>;
            }

            return UserObjectFactory<T>;
        }

        ObjectFactoryMap&    GetObjectTable() { return m_factories; }
        static ObjectFactory& Instance() { static ObjectFactory instance; return instance; }
    };

#define g_ObjectFactory ObjectFactory::Instance()
#define DECL_SERIALIZABLE(C) \
    static PfnUserObjectFactory __pfn##C##Factory_Internal = g_ObjectFactory.AddPrototype<C>();
#define DECL_SERIALIZABLE_NAMED(C, NAME) \
    static PfnUserObjectFactory __pfn##C##Factory_Internal = g_ObjectFactory.AddPrototype<C>(NAME);
}
#endif // OBJECTFACTORY_H_H

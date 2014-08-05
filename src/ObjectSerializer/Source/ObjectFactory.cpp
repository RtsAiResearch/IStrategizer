#ifndef OBJECTFACTORY_H
    #include "ObjectFactory.h"
#endif

#include <cassert>
#include <stdio.h>

//----------------------------------------------------------------------------------------------
ISerializable* ObjectFactory::Create(const string& p_typeName)
{
    _ASSERTE(!p_typeName.empty());
    ObjectFactoryMap::iterator where;
    if(m_cNameToFullNameTable.find(p_typeName) != m_cNameToFullNameTable.end())
    {
        where = m_factories.find(m_cNameToFullNameTable[p_typeName]);
    }
    else
    {
        where = m_factories.find(p_typeName);
    }

    if (where == m_factories.end())
    {
        char buffer[256];
        sprintf_s(buffer, sizeof(buffer), "Failed to retrieve object '%s'", p_typeName.c_str());
        throw std::exception(buffer);
    }

    return where->second();
}
//----------------------------------------------------------------------------------------------
const string& ObjectFactory::FromCName(const string& p_cName)
{
    _ASSERTE(m_cNameToFullNameTable.find(p_cName) != m_cNameToFullNameTable.end());
    return m_cNameToFullNameTable[p_cName];
}

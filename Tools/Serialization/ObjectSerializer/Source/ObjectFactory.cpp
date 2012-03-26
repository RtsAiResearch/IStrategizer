#ifndef OBJECTFACTORY_H
	#include "ObjectFactory.h"
#endif

#include <cassert>
#include <stdio.h>

//----------------------------------------------------------------------------------------------
void ObjectFactory::AddPrototype(UserObject* p_prototype)
{
    assert(p_prototype != NULL);
	string cname = p_prototype->CName();
	string tname = p_prototype->TypeName();
    m_cNameToFullNameTable[cname] = tname;
    m_prototypes[p_prototype->TypeName()] = p_prototype;
}
//----------------------------------------------------------------------------------------------
void ObjectFactory::AddPrototype(UserObject* p_prototype, char* p_fullName)
{
    assert(p_prototype != NULL);
    m_cNameToFullNameTable[p_prototype->CName()] = p_fullName;
    m_prototypes[p_fullName] = p_prototype;
}
//----------------------------------------------------------------------------------------------
UserObject* ObjectFactory::GetObject(const string& p_typeName)
{
    assert(!p_typeName.empty());
    ObjectTable::iterator where;
    if(m_cNameToFullNameTable.find(p_typeName) != m_cNameToFullNameTable.end())
    {
        where = m_prototypes.find(m_cNameToFullNameTable[p_typeName]);
    }
    else
    {
        where = m_prototypes.find(p_typeName);
    }

	if (where == m_prototypes.end())
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "Failed to retrieve object '%s'", p_typeName.c_str());
		throw std::exception(buffer);
	}

    return where->second;
}
//----------------------------------------------------------------------------------------------
const string& ObjectFactory::FromCName(const string& p_cName)
{
    assert(m_cNameToFullNameTable.find(p_cName) != m_cNameToFullNameTable.end());
    return m_cNameToFullNameTable[p_cName];
}
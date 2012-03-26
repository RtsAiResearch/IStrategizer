#include "ResourceDescription.h"
#include "GameEntity.h"
#include "MetaData.h"
#include "RtsGame.h"
#include "GameType.h"
#include <cmath>

using namespace IStrategizer;
using namespace MetaData;

void ResourceDescription::AddEntity(GameEntity *p_entity)
{
	EntityClassType typeId = p_entity->Type();
	GameType* pType = g_Game->GetEntityType(typeId);

	if (!pType)
		return;

	if (pType->Attr(ECATTR_IsPrimaryResource))
	{
		++m_numberOfSources[INDEX(RESOURCE_Primary, ResourceType)];
	}
	else if (pType->Attr(ECATTR_IsSecondaryResource))
	{
		++m_numberOfSources[INDEX(RESOURCE_Secondary, ResourceType)];
	}
}

void ResourceDescription::RemoveEntity(GameEntity *p_entity)
{
	EntityClassType typeId = p_entity->Type();
	GameType* pType = g_Game->GetEntityType(typeId);

	if (!pType)
		return;

	if(pType && pType->Attr(ECATTR_IsPrimaryResource))
	{
		--m_numberOfSources[INDEX(RESOURCE_Primary, ResourceType)];
	}
	else if (pType->Attr(ECATTR_IsSecondaryResource))
	{
		--m_numberOfSources[INDEX(RESOURCE_Secondary, ResourceType)];
	}
}

void ResourceDescription::Clear()
{
	for (int i = 0 ; i < COUNT(ResourceType) ; i++)
	{
		m_numberOfSources[i] = 0;
	}
}

double ResourceDescription::GetDistance(ResourceDescription *p_other)
{
	double dist = 0.0;
	for (int i = 0 ; i < COUNT(ResourceType) ; i++)
	{
		dist += abs(m_numberOfSources[i] - p_other->m_numberOfSources[i]);
	}
	return dist;
}
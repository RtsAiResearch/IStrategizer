#include "BuildingDescription.h"

#include "GameEntity.h"
#include "RtsGame.h"
#include "GameType.h"
#include <cmath>
#include <cassert>

using namespace IStrategizer;

void BuildingDescription::AddEntity(GameEntity *p_entity)
{
	EntityClassType typeId;
	GameType		*pType;

	assert(p_entity);

	typeId = p_entity->Type();
	pType = g_Game->GetEntityType(typeId);
	assert(pType);

	if (pType->Attr(ECATTR_IsBuilding))
	{
		++m_numberOfBuildings;
		if (pType->Attr(ECATTR_IsCritical))
			++m_numberOfCriticalBuildings;
	}
}
//----------------------------------------------------------------------------------------------
void BuildingDescription::RemoveEntity(GameEntity *p_entity)
{
	EntityClassType typeId;
	GameType		*pType;

	assert(p_entity);

	typeId = p_entity->Type();
	pType = g_Game->GetEntityType(typeId);
	assert(pType);

	if (pType->Attr(ECATTR_IsBuilding))
	{
		--m_numberOfBuildings;

		if (pType->Attr(ECATTR_IsCritical))
			--m_numberOfCriticalBuildings;
	}
}
//----------------------------------------------------------------------------------------------
void BuildingDescription::Clear()
{
	m_numberOfBuildings = 0;
	m_numberOfCriticalBuildings = 0;
}
//----------------------------------------------------------------------------------------------
float BuildingDescription::GetDistance(BuildingDescription *p_other)
{
	float dist = 0.0;

	dist += pow((float)(p_other->m_numberOfBuildings - m_numberOfBuildings), 2);
	dist += pow((float)(p_other->m_numberOfCriticalBuildings - m_numberOfCriticalBuildings), 2);

	return dist;
}
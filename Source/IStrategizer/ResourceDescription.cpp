#include "ResourceDescription.h"

#include "GameEntity.h"
#include "MetaData.h"
#include "RtsGame.h"
#include "GameType.h"
#include <cmath>

using namespace IStrategizer;

void ResourceDescription::InitializeAddressesAux()
{
	AddMemberAddress(3,
		&m_numberOfPrimary,
		&m_numberOfSecondary,
		&m_numberOfSupply);
}
//----------------------------------------------------------------------------------------------
void ResourceDescription::AddEntity(GameEntity *p_entity)
{
	EntityClassType typeId = p_entity->Type();
	GameType* pType = g_Game->GetEntityType(typeId);

	if (!pType)
		return;

	if (pType->Attr(ECATTR_IsPrimaryResource))
	{
		++m_numberOfPrimary;
	}
	else if (pType->Attr(ECATTR_IsSecondaryResource))
	{
		++m_numberOfSecondary;
	}
}
//----------------------------------------------------------------------------------------------
void ResourceDescription::RemoveEntity(GameEntity *p_entity)
{
	EntityClassType typeId = p_entity->Type();
	GameType* pType = g_Game->GetEntityType(typeId);

	if (!pType)
		return;

	if(pType && pType->Attr(ECATTR_IsPrimaryResource))
	{
		--m_numberOfPrimary;
	}
	else if (pType->Attr(ECATTR_IsSecondaryResource))
	{
		--m_numberOfSecondary;
	}
}
//----------------------------------------------------------------------------------------------
void ResourceDescription::Clear()
{
	m_numberOfPrimary = 0;
	m_numberOfSecondary = 0;
	m_numberOfSupply = 0;
}
//----------------------------------------------------------------------------------------------
double ResourceDescription::GetDistance(ResourceDescription *p_other)
{
	double dist = 0.0;
	dist += m_numberOfPrimary - p_other->m_numberOfPrimary;
	dist += m_numberOfSecondary - p_other->m_numberOfSecondary;
	dist += m_numberOfSupply - p_other->m_numberOfSupply;

	return dist;
}
//----------------------------------------------------------------------------------------------
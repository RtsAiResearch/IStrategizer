#include "ForceDescription.h"
#include "GameEntity.h"
#include "RtsGame.h"
#include "GameType.h"
#include <cmath>

using namespace IStrategizer;

void ForceDescription::InitializeAddressesAux()
{
	AddMemberAddress(3,
		&m_numberOfUnits,
		&m_totalHP,
		&m_totalDamage);
}

void ForceDescription::AddEntity(GameEntity *p_entity)
{
	m_numberOfUnits++;
	m_totalHP += p_entity->Attr(EOATTR_Health);
	m_totalDamage += g_Game->GetEntityType(p_entity->Type())->Attr(ECATTR_Attack);
	
}

void ForceDescription::RemoveEntity(GameEntity *p_entity)
{
	m_numberOfUnits--;
	m_totalHP -= p_entity->Attr(EOATTR_Health);
	m_totalDamage -= g_Game->GetEntityType(p_entity->Type())->Attr(ECATTR_Attack);
}

void ForceDescription::Clear()
{
	m_numberOfUnits = 0;
	m_totalHP = 0;
	m_totalDamage = 0;
}

double ForceDescription::GetDistance(ForceDescription *p_other)
{
	double dist = 0.0;
	dist += abs(m_numberOfUnits - p_other->m_numberOfUnits);
	dist += abs(m_totalHP - p_other->m_totalHP);
	dist += abs(m_totalDamage - p_other->m_totalDamage);

	return dist;
}
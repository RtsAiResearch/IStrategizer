#include "ForceDescription.h"
#include "GameEntity.h"
#include "RtsGame.h"
#include "GameType.h"
#include <cmath>

using namespace IStrategizer;

void ForceDescription::AddEntity(RtsGame& p_RtsGame, GameEntity *p_entity)
{
    m_numberOfUnits++;
    m_totalHP += p_entity->Attr(EOATTR_Health);
    m_totalDamage += p_RtsGame.GetEntityType(p_entity->Type())->Attr(ECATTR_Attack);
    
}

void ForceDescription::RemoveEntity(RtsGame& p_RtsGame, GameEntity *p_entity)
{
    m_numberOfUnits--;
    m_totalHP -= p_entity->Attr(EOATTR_Health);
    m_totalDamage -= p_RtsGame.GetEntityType(p_entity->Type())->Attr(ECATTR_Attack);
}

void ForceDescription::Clear()
{
    m_numberOfUnits = 0;
    m_totalHP = 0;
    m_totalDamage = 0;
}

float ForceDescription::GetDistance(ForceDescription *p_other)
{
    float dist = 0.0;

    dist += pow((float)(m_numberOfUnits - p_other->m_numberOfUnits), 2);
    dist += pow((float)(m_totalHP - p_other->m_totalHP), 2);
    dist += pow((float)(m_totalDamage - p_other->m_totalDamage), 2);

    return dist;
}

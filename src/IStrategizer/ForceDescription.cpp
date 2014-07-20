#include "ForceDescription.h"
#include "GameEntity.h"
#include "RtsGame.h"
#include "GameType.h"
#include <cmath>

using namespace IStrategizer;

void ForceDescription::AddEntity(GameEntity *p_entity)
{
    m_numberOfUnits++;
    m_totalHP += p_entity->Attr(EOATTR_Health);
    m_totalDamage += g_Game->GetEntityType(p_entity->TypeId())->Attr(ECATTR_GroundAttack);
}
//////////////////////////////////////////////////////////////////////////
void ForceDescription::RemoveEntity(GameEntity *p_entity)
{
    m_numberOfUnits--;
    m_totalHP -= p_entity->Attr(EOATTR_Health);
    m_totalDamage -= g_Game->GetEntityType(p_entity->TypeId())->Attr(ECATTR_GroundAttack);
}
//////////////////////////////////////////////////////////////////////////
void ForceDescription::Clear()
{
    m_numberOfUnits = DONT_CARE;
    m_totalHP = DONT_CARE;
    m_totalDamage = DONT_CARE;
}
//////////////////////////////////////////////////////////////////////////
float ForceDescription::GetDistance(ForceDescription *p_other) const
{
    float dist = 0.0;

    if (m_numberOfUnits != DONT_CARE && p_other->m_numberOfUnits != DONT_CARE)
        dist += pow((float)(m_numberOfUnits - p_other->m_numberOfUnits), 2);
    
    if (m_totalHP != DONT_CARE && p_other->m_totalHP != DONT_CARE)
        dist += pow((float)(m_totalHP - p_other->m_totalHP), 2);
    
    if (m_totalDamage != DONT_CARE && p_other->m_totalDamage != DONT_CARE)
        dist += pow((float)(m_totalDamage - p_other->m_totalDamage), 2);

    return dist;
}

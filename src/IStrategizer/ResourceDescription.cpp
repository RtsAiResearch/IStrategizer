#include "ResourceDescription.h"

#include "GameEntity.h"
#include "MetaData.h"
#include "RtsGame.h"
#include "GameType.h"
#include <cmath>

using namespace IStrategizer;

//----------------------------------------------------------------------------------------------
void ResourceDescription::AddEntity(GameEntity *p_entity)
{
    EntityClassType typeId = p_entity->TypeId();
    GameType* pType = g_Game->GetEntityType(typeId);

    if (!pType)
        return;

    if (pType->P(TP_IsPrimaryResource))
    {
        ++m_numberOfPrimary;
    }
    else if (pType->P(TP_IsSecondaryResource))
    {
        ++m_numberOfSecondary;
    }
}
//----------------------------------------------------------------------------------------------
void ResourceDescription::RemoveEntity(GameEntity *p_entity)
{
    EntityClassType typeId = p_entity->TypeId();
    GameType* pType = g_Game->GetEntityType(typeId);

    if (!pType)
        return;

    if(pType && pType->P(TP_IsPrimaryResource))
    {
        --m_numberOfPrimary;
    }
    else if (pType->P(TP_IsSecondaryResource))
    {
        --m_numberOfSecondary;
    }
}
//----------------------------------------------------------------------------------------------
void ResourceDescription::Clear()
{
    m_numberOfPrimary = DONT_CARE;
    m_numberOfSecondary = DONT_CARE;
    m_numberOfSupply = DONT_CARE;
}
//----------------------------------------------------------------------------------------------
float ResourceDescription::GetDistance(ResourceDescription *p_other) const
{
    float dist = 0.0;

    if (m_numberOfPrimary != DONT_CARE && p_other->m_numberOfPrimary != DONT_CARE)
        dist += pow((float)(m_numberOfPrimary - p_other->m_numberOfPrimary), 2);
    
    if (m_numberOfSecondary != DONT_CARE && p_other->m_numberOfSecondary != DONT_CARE)
        dist += pow((float)(m_numberOfSecondary - p_other->m_numberOfSecondary), 2);
    
    if (m_numberOfSupply != DONT_CARE && p_other->m_numberOfSupply != DONT_CARE)
        dist += pow((float)(m_numberOfSupply - p_other->m_numberOfSupply), 2);

    return dist;
}
//----------------------------------------------------------------------------------------------

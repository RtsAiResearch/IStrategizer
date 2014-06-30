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
    GameType *pType;

    _ASSERTE(p_entity);

    typeId = p_entity->Type();
    pType = g_Game->GetEntityType(typeId);
    _ASSERTE(pType);

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
    GameType *pType;

    _ASSERTE(p_entity);

    typeId = p_entity->Type();
    pType = g_Game->GetEntityType(typeId);
    _ASSERTE(pType);

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
    m_numberOfBuildings = DONT_CARE;
    m_numberOfCriticalBuildings = DONT_CARE;
}
//----------------------------------------------------------------------------------------------
float BuildingDescription::GetDistance(BuildingDescription *p_other) const
{
    float dist = 0.0;

    if (m_numberOfBuildings != DONT_CARE && p_other->m_numberOfBuildings != DONT_CARE)
        dist += pow((float)(p_other->m_numberOfBuildings - m_numberOfBuildings), 2);

    if (m_numberOfCriticalBuildings != DONT_CARE && p_other->m_numberOfCriticalBuildings != DONT_CARE)
        dist += pow((float)(p_other->m_numberOfCriticalBuildings - m_numberOfCriticalBuildings), 2);

    return dist;
}

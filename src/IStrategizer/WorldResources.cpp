#include "WorldResources.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "PlayerResources.h"
#include "EngineAssist.h"

using namespace IStrategizer;

bool WorldResources::Acquire()
{
    return g_Game->Self()->Resources()->Lock(this);
}
//////////////////////////////////////////////////////////////////////////
bool WorldResources::Release()
{
    g_Game->Self()->Resources()->Unlock(this);

    return true;
}
//////////////////////////////////////////////////////////////////////////
WorldResources WorldResources::FromEntity(int p_entityOrResearchType)
{
    WorldResources worldResources;
    g_Assist.GetPrerequisiteResources(p_entityOrResearchType, PLAYER_Self, worldResources);

    return worldResources;
}
//////////////////////////////////////////////////////////////////////////
void WorldResources::Set(const WorldResources* pOther)
{
    m_supply = pOther->m_supply;
    m_secondary = pOther->m_secondary;
    m_primary = pOther->m_primary;
}
//////////////////////////////////////////////////////////////////////////
bool WorldResources::IsNull()
{
    return m_supply < 0 ||
        m_primary < 0 ||
        m_secondary < 0;
}
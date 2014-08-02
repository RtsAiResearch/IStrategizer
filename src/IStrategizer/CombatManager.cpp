#include "CombatManager.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"

using namespace IStrategizer;

void CombatManager::Init()
{
    // Each army controls a specific category of units
    m_frontLinesArmy.SetControlType(true, false, false);
    m_reinforcementsArmy.SetControlType(true, false, false);
    m_brokenArmy.SetControlType(false, true, false);

    auto baseLocation = g_Game->Self()->StartLocation();
    // Broken army stand at base waiting for repair/heal
    m_brokenArmy.Stand(baseLocation);

    // FrontLines/Reinf army defend base waiting
    m_frontLinesArmy.Defend(baseLocation);
    m_reinforcementsArmy.Defend(baseLocation);
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::Update()
{
    m_brokenArmy.ReleaseHealthyEntities();

    // Look for new entities to control this frame
    for (auto& entityR : g_Game->Self()->Entities())
    {
        m_reinforcementsArmy.TryControlEntity(entityR.second->Id());
        m_brokenArmy.TryControlEntity(entityR.second->Id());
    }

    m_frontLinesArmy.Update();
    m_reinforcementsArmy.Update();
    m_brokenArmy.Update();
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::AttackArea(_In_ Vector2 pos)
{
    _ASSERTE(!pos.IsInf());
    LogInfo("Attacking area %s", pos.ToString().c_str());

    m_reinforcementsArmy.ReleaseArmy();
    m_frontLinesArmy.ControlNewArmy();
    m_frontLinesArmy.Attack(pos);
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::DefendArea(_In_ Vector2 pos)
{
    _ASSERTE(!pos.IsInf());
    LogInfo("Defending area %s", pos.ToString());

    m_frontLinesArmy.Defend(pos);
    m_reinforcementsArmy.Defend(pos);
}

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
    bool newReinforcements = false;
    bool newBrokens = false;

    // Look for new entities to control this frame
    for (auto& entityR : g_Game->Self()->Entities())
    {
        if (!newReinforcements && m_reinforcementsArmy.CanControl(entityR.second))
            newReinforcements = true;
        else if (!newBrokens && m_brokenArmy.CanControl(entityR.second))
            newBrokens = true;

        // We found what we are looking for, no need to continue search
        if (newBrokens && newReinforcements)
            break;
    }

    if (newReinforcements)
        m_reinforcementsArmy.ControlNewArmy();

    if (newBrokens)
        m_brokenArmy.ControlNewArmy();

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

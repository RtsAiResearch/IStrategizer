#include "CombatManager.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"

using namespace IStrategizer;

void CombatManager::Init()
{
    // By default we defend our base
    DefendArea(g_Game->Self()->StartLocation());
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::Update()
{
    bool newReinforcements = false;
    bool newBrokens = false;

    // Look for new entities to control this frame
    for (auto& entityR : g_Game->Self()->Entities())
    {
        // Only include health unit in the army, other units exclude
        // for now
        // Later they should self heal or be repaired by workers
        if (!m_reinforcementsArmy.IsControllingEntity(entityR.first) &&
            m_reinforcementsArmy.CanControl(entityR.second))
        {
            if (EntityController::IsOnCriticalHP(entityR.second))
                newBrokens = true;
            else
                newReinforcements = true;

            // We found what we are looking for, no need to continue search
            if (newBrokens && newReinforcements)
                break;
        }
    }

    if (newReinforcements)
        m_reinforcementsArmy.ControlNewArmy(true, false);

    if (newBrokens)
        m_brokenArmy.ControlNewArmy(false, true);

    m_reinforcementsArmy.Update();
    m_frontLinesArmy.Update();
    m_brokenArmy.Update();
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::AttackArea(_In_ Vector2 pos)
{
    _ASSERTE(!pos.IsInf());
    LogInfo("Attacking area %s", pos.ToString());

    m_reinforcementsArmy.ReleaseArmy();
    m_frontLinesArmy.ControlNewArmy(true, false);
    m_frontLinesArmy.AttackArea(pos);
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::DefendArea(_In_ Vector2 pos)
{
    _ASSERTE(!pos.IsInf());
    LogInfo("Defending area %s", pos.ToString());

    m_frontLinesArmy.DefendArea(pos);
    m_reinforcementsArmy.DefendArea(pos);

    // If we are defending our base, we stand or lose
    // Use all units to survive
    if (pos == g_Game->Self()->StartLocation())
        m_brokenArmy.DefendArea(pos);
}

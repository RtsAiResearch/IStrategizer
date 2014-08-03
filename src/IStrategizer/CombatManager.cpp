#include "CombatManager.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "WorldMap.h"
#include "IStrategizerEx.h"

using namespace IStrategizer;

void CombatManager::Init()
{
    // Each army controls a specific category of units
    m_frontLinesArmy.SetControlType(false, false);
    m_reinforcementsArmy.SetControlType(false, false);
    m_brokenArmy.SetControlType(true, false);

    auto baseStartLoc = g_Game->Self()->StartLocation();
    Vector2F deltaF = (g_Engine->BaseHeadDirection() * 320.0f);
    Vector2 delta((int)deltaF.X, (int)deltaF.Y);
    Vector2F brokenDeltaF = (g_Engine->BaseHeadDirection() * 196.0f);
    Vector2 brokenDelta((int)brokenDeltaF.X, (int)brokenDeltaF.Y);

    // Broken army stand at base waiting for repair/heal
    m_brokenArmy.Stand(baseStartLoc + brokenDelta);
    // FrontLines/Reinf army defend base waiting
    m_frontLinesArmy.Defend(baseStartLoc + delta);
    m_reinforcementsArmy.Defend(baseStartLoc + delta);
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::Update()
{
    m_brokenArmy.ReleaseHealthyEntities();

    m_reinforcementsArmy.TryControlArmy(false);
    m_brokenArmy.TryControlArmy(false);

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
    m_frontLinesArmy.TryControlArmy(true);
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

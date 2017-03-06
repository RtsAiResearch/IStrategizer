#include "CombatManager.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "WorldMap.h"
#include "IStrategizerEx.h"
#include "ArmyFSM.h"

using namespace IStrategizer;

void CombatManager::Init()
{
    // Each army controls a specific category of units
    m_frontLinesArmy.SetControlType(false, false);
    m_brokenArmy.SetControlType(true, false);

    //
    // Compute the armies locations
    //
    auto baseStartLoc = g_Game->Self()->StartLocation();
    LogInfo("BaseLocation: %s", baseStartLoc.ToString().c_str());
    
    // Broken army stand at base waiting for repair/heal all the time
    Vector2F brokenTranslateF = (g_Engine->BaseHeadDirection() * float(BrokenArmyBaseDistance));
    Vector2 brokenTranslate((int)brokenTranslateF.X, (int)brokenTranslateF.Y);
    m_brokenArmy.Stand(baseStartLoc + brokenTranslate);

    // FrontLines/Reinforcements army defend base at the armies base
    Vector2F othersTranslateF = (g_Engine->BaseHeadDirection() * float(FrontlinesArmyBaseDistance));
    Vector2 othersTranslate((int)othersTranslateF.X, (int)othersTranslateF.Y);
    m_armiesBaseLoc = baseStartLoc + othersTranslate;
    DefendBase();
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::Update()
{
    m_brokenArmy.ReleaseHealthyEntities();

    m_frontLinesArmy.TryControlArmy(false);
    m_brokenArmy.TryControlArmy(false);

    m_frontLinesArmy.Update();
    m_brokenArmy.Update();

    // Army defeated, bring it back
    if (m_frontLinesArmy.Logic()->TypeId() == AttackMoveArmyFSM::TypeID &&
        !m_frontLinesArmy.IsControllingArmy())
    {
        m_frontLinesArmy.Defend(m_armiesBaseLoc);
    }
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::AttackEnemy(_In_ Vector2 pos)
{
    _ASSERTE(!pos.IsInf());
    LogInfo("Attacking area %s", pos.ToString().c_str());

    m_frontLinesArmy.TryControlArmy(false);
    m_frontLinesArmy.Attack(pos);

    m_currOrder = CMBTMGR_Attack;
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::DefendBase()
{
    LogInfo("Defending area %s", m_armiesBaseLoc.ToString().c_str());

    m_frontLinesArmy.Defend(m_armiesBaseLoc);
    m_currOrder = CMBTMGR_Defend;
}
//////////////////////////////////////////////////////////////////////////
void CombatManager::DebugDraw()
{
    m_frontLinesArmy.DebugDraw();
    m_brokenArmy.DebugDraw();
}
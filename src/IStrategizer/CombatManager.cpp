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
    for (auto& entityR : g_Game->Self()->Entities())
    {
        // Only include health unit in the army, other units exclude
        // for now
        // Later they should self heal or be repaired by workers
        if (!EntityController::IsOnCriticalHP(entityR.second))
            m_armyCtrlr.ControlEntity(entityR.first);
    }

    m_armyCtrlr.Update();
}
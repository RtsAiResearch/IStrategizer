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
    bool newEntitiesToControl = false;
    
    // Look for new entities to control this frame
    for (auto& entityR : g_Game->Self()->Entities())
    {
        // Only include health unit in the army, other units exclude
        // for now
        // Later they should self heal or be repaired by workers
        if (!m_armyCtrlr.IsControllingEntity(entityR.first) &&
            m_armyCtrlr.CanControl(entityR.second))
        {
            newEntitiesToControl = true;
            break;
        }
    }

    if (newEntitiesToControl)
        m_armyCtrlr.ControlArmy();

    m_armyCtrlr.Update();
}
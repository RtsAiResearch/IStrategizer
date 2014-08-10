#include "DefinitionCrossMapping.h"
#include "BWAPI.h"
#include <sstream>
#include <fstream>

using namespace BWAPI;
using namespace std;

const int TechIdOffset = 127;

DefinitionCrossMapping::DefinitionCrossMapping() : m_initialized(false) {}
//////////////////////////////////////////////////////////////////////////
void DefinitionCrossMapping::Init()
{
    InitEntityIdents();
    InitUpgradeIdents();
    InitTechIdents();
    InitActions();

    m_initialized = true;
}
//////////////////////////////////////////////////////////////////////////
void DefinitionCrossMapping::InitEntityIdents()
{
    vector< pair<TID, TName> > entities;

    for(UnitType::set::iterator i = UnitTypes::allUnitTypes().begin();
        i != UnitTypes::allUnitTypes().end();
        ++i)
    {
        entities.push_back(make_pair((*i).getID(), (*i).getName()));
    }

    EntityIdentMapping = CrossMap<TID, TName>(entities);
}
//////////////////////////////////////////////////////////////////////////-
void DefinitionCrossMapping::InitUpgradeIdents()
{
    vector< pair<TID, TName> > m_upgrades;

    for(UpgradeType::set::iterator i = UpgradeTypes::allUpgradeTypes().begin();
        i != UpgradeTypes::allUpgradeTypes().end();
        ++i)
    {
        m_upgrades.push_back(make_pair((*i).getID(), (*i).getName()));
    }

    UpgradeIdentMapping = CrossMap<TID, TName>(m_upgrades);
}
//////////////////////////////////////////////////////////////////////////-
void DefinitionCrossMapping::InitTechIdents()
{
    vector< pair<TID, TName> > m_teches;

    for(TechType::set::iterator i = TechTypes::allTechTypes().begin();
        i != TechTypes::allTechTypes().end();
        ++i)
    {
        m_teches.push_back(make_pair((*i).getID(), (*i).getName()));
    }

    TechIdentMapping = CrossMap<TID, TName>(m_teches);
}
//////////////////////////////////////////////////////////////////////////-
void DefinitionCrossMapping::InitActions()
{
    vector< pair<TID, ActionType> > actions;

    actions.push_back(make_pair(Orders::Train.getID(), ACTIONEX_Train));
    
    actions.push_back(make_pair(Orders::AttackMove.getID(), ACTIONEX_AttackGround));
    actions.push_back(make_pair(Orders::AttackUnit.getID(), ACTIONEX_AttackEntity));

    actions.push_back(make_pair(Orders::ResearchTech.getID(), ACTIONEX_Research));
    actions.push_back(make_pair(Orders::Upgrade.getID(), ACTIONEX_Research));

    actions.push_back(make_pair(Orders::PlaceBuilding.getID(), ACTIONEX_Build));
    actions.push_back(make_pair(Orders::PlaceAddon.getID(), ACTIONEX_Build));
    
    actions.push_back(make_pair(Orders::MoveToMinerals.getID(), ACTIONEX_GatherResource));
    actions.push_back(make_pair(Orders::MiningMinerals.getID(), ACTIONEX_GatherResource));
    actions.push_back(make_pair(Orders::WaitForMinerals.getID(), ACTIONEX_GatherResource));
    actions.push_back(make_pair(Orders::ReturnMinerals.getID(), ACTIONEX_GatherResource));
    
    actions.push_back(make_pair(Orders::MoveToGas.getID(), ACTIONEX_GatherResource));
    actions.push_back(make_pair(Orders::HarvestGas.getID(), ACTIONEX_GatherResource));
    actions.push_back(make_pair(Orders::WaitForGas.getID(), ACTIONEX_GatherResource));
    actions.push_back(make_pair(Orders::ReturnGas.getID(), ACTIONEX_GatherResource));

    ActionMapping = CrossMap<TID, IStrategizer::ActionType>(actions);
}


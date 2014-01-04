#include "DefinitionCrossMapping.h"
#include "BWAPI.h"
#include <sstream>
#include <fstream>

using namespace BWAPI;
using namespace std;

const int TechIdOffset = 127;

DefinitionCrossMapping::DefinitionCrossMapping() : m_initialized(false) {}
//---------------------------------------------------------------------------------------------
void DefinitionCrossMapping::Init()
{
    if (m_initialized)
    {
        // Because we depend on player actual Id in the game, and this class is singleton
        // The starcraft ame may change player ids from game run to another
        PlayerMapping.Clear();
        InitPlayers();
        return;
    }

    InitEntities();
    InitUpgrades();
    InitTeches();
    InitPlayers();
    InitActions();
    InitEntityIdents();
    InitUpgradeIdents();
    InitTechIdents();
  InitActions();

    m_initialized = true;
}
//---------------------------------------------------------------------------------------------
void DefinitionCrossMapping::InitEntities()
{
    vector< pair<TID, EntityClassType> > entities;
    const char *name;

    int currentId = (int)ECLASS_START;
    for(UnitType::set::iterator i = UnitTypes::allUnitTypes().begin();
        i != UnitTypes::allUnitTypes().end();
        ++i)
    {
        entities.push_back(make_pair((*i).getID(), (EntityClassType)currentId));

        name = (*i).getName().c_str();
        Enums[currentId] = _strdup(name);

        ++currentId;
    }

    EntityMapping = CrossMap<TID, EntityClassType>(entities);
}
//---------------------------------------------------------------------------------------------
void DefinitionCrossMapping::InitUpgrades()
{
    vector< pair<TID, ResearchType> > upgrades;
    const char *name;

    int currentId = (int)RESEARCH_START;
    assert(UpgradeTypes::allUpgradeTypes().size() < (COUNT(ResearchType) / 2));

    for(UpgradeType::set::iterator i = UpgradeTypes::allUpgradeTypes().begin();
        i != UpgradeTypes::allUpgradeTypes().end();
        ++i)
    {
        upgrades.push_back(make_pair((*i).getID(), (ResearchType)currentId));

        name = (*i).getName().c_str();
        Enums[currentId] = _strdup(name);

        ++currentId;
    }

    UpgradeMapping = CrossMap<TID, ResearchType>(upgrades);
}
//--------------------------------------------------------------------------------------------------------------------------
void DefinitionCrossMapping::InitTeches()
{
    vector< pair<TID, ResearchType> > m_teches;
    const char *name;

    int currentId = (int)RESEARCH_START + TechIdOffset;
    assert(TechTypes::allTechTypes().size() < (COUNT(ResearchType) / 2));

    for(TechType::set::iterator i = TechTypes::allTechTypes().begin();
        i != TechTypes::allTechTypes().end();
        ++i)
    {
        m_teches.push_back(make_pair((*i).getID(), (ResearchType)currentId));

        name = (*i).getName().c_str();
        Enums[currentId] = _strdup(name);

        ++currentId;
    }

    TechMapping = CrossMap<TID, ResearchType>(m_teches);
}
//----------------------------------------------------------------------------------------------
void DefinitionCrossMapping::InitPlayers()
{
    vector< pair<TID, IStrategizer::PlayerType> > m_players;
    const Playerset &players = Broodwar->getPlayers();
    BWAPI::Player pPlayer;

    for (Playerset::iterator i = players.begin();
    i != players.end();
        ++i)
    {
        pPlayer = (*i);

    // FIXME: PLAYER_Self is not valid if there are more than 1 human player
    // for example in replays or network game
    // Player types should be removed from the engine and only Player IDs should be used
        if(pPlayer->getType().getID() == PlayerTypes::Player.getID())
            m_players.push_back(make_pair(pPlayer->getID(), PLAYER_Self));
        else if(pPlayer->getType().getID() == PlayerTypes::Computer.getID())
            m_players.push_back(make_pair(pPlayer->getID(), PLAYER_Enemy));
        else if(pPlayer->isNeutral())
            m_players.push_back(make_pair(pPlayer->getID(), PLAYER_Neutral));
    }

    PlayerMapping = CrossMap<TID, IStrategizer::PlayerType>(m_players);
}
//----------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------
bool DefinitionCrossMapping::ExportGameIds(string p_exportPath)
{
    fstream pen;
    stringstream stream;
    pen.open(p_exportPath.c_str(), ios::out);

    if (!pen.is_open())
        return false;

    vector<TID> ids;
    EntityIdentMapping.FirstValues(ids);
    for(unsigned i = 0; i < ids.size(); ++i)
    {
        stream << EntityIdentMapping.GetByFirst(ids[i]);
        if(i == 0)
            stream << ' ' << EntityMapping.GetByFirst(ids[i]);
    }

    stream << endl;

    ids.clear();
    UpgradeIdentMapping.FirstValues(ids);
    for(unsigned i = 0; i < ids.size(); ++i)
    {
        stream << UpgradeIdentMapping.GetByFirst(ids[i]);
        if(i == 0)
            stream << ' ' << UpgradeMapping.GetByFirst(ids[i]);
    }

    stream << endl;

    ids.clear();
    TechIdentMapping.FirstValues(ids);
    for(unsigned i = 0; i < ids.size(); ++i)
    {
        stream << TechIdentMapping.GetByFirst(ids[i]);
        if(i == 0)
            stream << ' ' << TechMapping.GetByFirst(ids[i]);
    }

    pen << stream.str();
    pen.close();

    return true;
}
//----------------------------------------------------------------------------------------------
bool DefinitionCrossMapping::ExportAllIds(string p_exportPath)
{
    fstream pen;
    stringstream stream;
    const char* name;
    pen.open(p_exportPath.c_str(), ios::out);

    if (!pen.is_open())
        return false;

    for (unsigned id = 0; id < ENUMS_SIZE; ++id)
    {
        name = Enums[id];

        // Has a name defined
        if (name != nullptr)
        {
            stream << id << ' ' << name << endl;
        }
    }

    pen << stream.str();
    pen.close();

    return true;
}
//----------------------------------------------------------------------------------------------
void DefinitionCrossMapping::EntityTypes(vector<EntityClassType>& p_entityTypes)
{
    EntityMapping.SecondValues(p_entityTypes);
}
//----------------------------------------------------------------------------------------------
void DefinitionCrossMapping::ResearchTypes(vector<IStrategizer::ResearchType>& p_researchTypes)
{
    UpgradeMapping.SecondValues(p_researchTypes);
    TechMapping.SecondValues(p_researchTypes, true);
}
//----------------------------------------------------------------------------------------------
void DefinitionCrossMapping::InitActions()
{
  vector< pair<TID, ActionType> > actions;

  actions.push_back(make_pair(Orders::AttackMove.getID(), ACTIONEX_AttackGround));
  actions.push_back(make_pair(Orders::AttackUnit.getID(), ACTIONEX_AttackEntity));
  actions.push_back(make_pair(Orders::ResearchTech.getID(), ACTIONEX_Research));
  actions.push_back(make_pair(Orders::Upgrade.getID(), ACTIONEX_Research));
  actions.push_back(make_pair(Orders::PlaceBuilding.getID(), ACTIONEX_Build));
  actions.push_back(make_pair(Orders::Train.getID(), ACTIONEX_Train));
  actions.push_back(make_pair(Orders::Move.getID(), ACTIONEX_Move));

  ActionMapping = CrossMap<TID, IStrategizer::ActionType>(actions);
}

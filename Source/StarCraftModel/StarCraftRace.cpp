#include "StarCraftRace.h"
#include "EngineData.h"
#include "DefinitionCrossMapping.h"

using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

// Been determined empirically by analyzing professional games. 
// Refer to paper "Build Order Optimization in StarCraft" page 3
const float StarCraftRace::MineralsPerWorkerPerFrame = 0.045f;
const float StarCraftRace::GasPerWorkerPerFrame = 0.07f;

//----------------------------------------------------------------------------------------------
StarCraftRace::StarCraftRace(BWAPI::Player pPlayer) :
    m_pPlayer(pPlayer)
{
    TID typeId = m_pPlayer->getRace().getWorker().getID();
    m_workerTypeId = g_Database.EntityMapping.GetByFirst(typeId);

    typeId = m_pPlayer->getRace().getCenter().getID();
    m_baseTypeId = g_Database.EntityMapping.GetByFirst(typeId);
}
//----------------------------------------------------------------------------------------------
std::string StarCraftRace::Name() const { return m_pPlayer->getRace().getName(); }
//----------------------------------------------------------------------------------------------
int StarCraftRace::BaseSupplyAmount() const
{
    _ASSERTE(m_pPlayer->getRace() == Races::Terran);
    return 10;
}
//----------------------------------------------------------------------------------------------
int StarCraftRace::SupplyBuildingSupplyAmount() const
{
    _ASSERTE(m_pPlayer->getRace() == Races::Terran);
    return 8;
}
//----------------------------------------------------------------------------------------------
float StarCraftRace::GetResourceConsumbtionRatePerWorker(ResourceType p_id) const
{
    _ASSERTE(m_pPlayer->getRace() == Races::Terran);

    switch(p_id)
    {
    case RESOURCE_Primary:
        return MineralsPerWorkerPerFrame;
    case RESOURCE_Secondary:
        return GasPerWorkerPerFrame;
    default:
        throw InvalidParameterException(XcptHere);
    }
}
//----------------------------------------------------------------------------------------------
EntityClassType StarCraftRace::GetResourceSource(ResourceType p_type) const
{
    // This method is hard-coded for Terran, ideally it should take race type.
    _ASSERTE(m_pPlayer->getRace() == Races::Terran);

    switch(p_type)
    {
    case RESOURCE_Primary:
        return g_Database.EntityMapping.GetByFirst(UnitTypes::Resource_Mineral_Field.getID());

    case RESOURCE_Secondary:
        return g_Database.EntityMapping.GetByFirst(UnitTypes::Terran_Refinery.getID());

    case RESOURCE_Supply:
        return g_Database.EntityMapping.GetByFirst(UnitTypes::Terran_Supply_Depot.getID());

    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
    }
}

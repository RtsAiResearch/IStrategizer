#include "GameType.h"
#include "WorldResources.h"
#include "RtsGame.h"
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace std;

DECL_SERIALIZABLE(GameType);

void GameType::Init()
{
    // BWAPI SPECIFIC
    // Positions are measured in pixels and are the highest resolution
    // Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
    // Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels.
    // These are called build tiles because buildability data is available at this resolution, and correspond to the tiles seen in game.
    // For example, a Command Center occupies an area of 4x3 build tiles. Build tiles can be specified in TilePosition objects.
    // We will use walk tiles as a measure of positions units across IStrategizer
    // See: http://code.google.com/p/bwapi/wiki/Misc

    // The supply amount is doubled, divide over two.
    m_requiredResources = WorldResources(m_pUnitType->SupplyRequired() / 2, m_pUnitType->GasPrice(), m_pUnitType->MineralsPrice());

    P(TP_CanAttack, m_pUnitType->CanAttack());
    P(TP_CanBuild, m_pUnitType->CanBuildAddOn() || m_pUnitType->IsWorker());
    P(TP_IsBuilding, m_pUnitType->IsBuilding());
    P(TP_IsProducer, m_pUnitType->CanProduce());
    P(TP_IsWorker, m_pUnitType->IsWorker());
    P(TP_IsAttacker, m_pUnitType->CanAttack() && !m_pUnitType->IsWorker());
    P(TP_MaxHp, m_pUnitType->MaxHitpoints());
    P(TP_IsPrimaryResource, m_pUnitType->IsMineralsField());
    P(TP_IsSecondaryResource, m_pUnitType->IsGasField());
    P(TP_IsSupply, (m_pUnitType->SupplyProvided() > 0));
    // FIXME: mention critical buildings
    P(TP_LineOfSight, m_pUnitType->SightRange());
    P(TP_BuildTime, m_pUnitType->BuildTime());
    P(TP_CanBuildingExpand, m_pUnitType->CanBuildAddOn());
    P(TP_IsResoureDepot, m_pUnitType->IsResourceDepot());

    if (m_pUnitType->CanBuildAddOn())
        P(TP_BuildingExpansionIncrement, 64);
    else
        P(TP_BuildingExpansionIncrement, 0);

    // Convert build tiles to walk tiles
    if (m_pUnitType->IsBuilding())
    {
        P(TP_Width, m_pUnitType->TileWidth() * g_GameImpl->MapTileSize());
        P(TP_Height, m_pUnitType->TileHeight() * g_GameImpl->MapTileSize());
    }
    else
    {
        P(TP_Width, m_pUnitType->DimensionLeft() * 2);
        P(TP_Height, m_pUnitType->DimensionUp() * 2);
    }

    GameWeaponType groundWeapon = m_pUnitType->GroundWeapon();
    GameWeaponType airWeapon = m_pUnitType->AirWeapon();

    P(TP_AirRange, airWeapon.MaxRange);
    P(TP_AirAttack, airWeapon.DamageAmount);
    P(TP_GroundRange, groundWeapon.MaxRange);
    P(TP_GroundAttack, groundWeapon.DamageAmount);
    P(TP_IsSpecialBuilding, m_pUnitType->IsRefinery() || m_pUnitType->IsAddon());
    P(TP_IsMelee, groundWeapon.MaxRange <= g_GameImpl->MapTileSize());
}
//////////////////////////////////////////////////////////////////////////
EntityClassType GameType::GetBuilderType() const
{
    if (m_pUnitType->IsAddon())
    {
        return m_pUnitType->WhatBuilds()->EngineId();
    }
    else
    {
        _ASSERTE(m_pUnitType->IsBuilding());
        return m_pUnitType->Race()->WorkerType()->EngineId();
    }
}
//////////////////////////////////////////////////////////////////////////
void GameType::GetRequirements(vector<ResearchType>& researches, map<EntityClassType, unsigned>& buildings) const
{
    if (m_pUnitType->RequiredTech() != nullptr)
    {
        researches.push_back(m_pUnitType->RequiredTech()->EngineId());
    }

    auto requiredUnits = m_pUnitType->RequiredUnits();
    for (int i = 0; i < requiredUnits->Size(); ++i)
    {
        auto unitR = requiredUnits->At(i);
        buildings[unitR.Key->EngineId()] = unitR.Value;
    }
}
//////////////////////////////////////////////////////////////////////////
EntityClassType GameType::SourceEntity() const
{
    return m_pUnitType->WhatBuilds()->EngineId();
}

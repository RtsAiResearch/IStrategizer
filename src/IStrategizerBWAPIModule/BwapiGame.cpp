#include "BwapiGame.h"
#include <map>
#include <BWAPI.h>

using namespace BWAPI;
using namespace IStrategizer;
using namespace std;

// Global static types initialized once per BWAPI client lifetime
map<TID, IGameUnitType*> g_BwapiUnitTypes;
map<TID, IGameTechType*> g_BwapiTechTypes;
map<TID, IGameUpgradeType*> g_BwapiUpgradeTypes;
map<TID, IGameRace*> g_BwapiRaces;

BWAPI::Color BwapiColorFrom(GameDrawColor c)
{
    switch (c)
    {
    case GCLR_Red:
        return Colors::Red;
    case GCLR_Green:
        return Colors::Green;
    case GCLR_Blue:
        return Colors::Blue;
    case GCLR_Yellow:
        return Colors::Yellow;
    case GCLR_Orange:
        return Colors::Orange;
    case GCLR_Cyan:
        return Colors::Cyan;
    case GCLR_Purple:
        return Colors::Purple;
    case GCLR_White:
    default:
        return Colors::White;
    }
}

class BwapiUnitType : public IGameUnitType
{
public:
    BwapiUnitType(BWAPI::UnitType type) :
        m_type(type)
    {
    }

    virtual TID GameId() const
    {
        return m_type.getID();
    }

    virtual EntityClassType EngineId() const
    {
        return (EntityClassType)((int)ECLASS_START + m_type.getID());
    }

    virtual const IGameUnitType* WhatBuilds() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual const IGameTechType* RequiredTech() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual GameUnitTypeToCountList RequiredUnits() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool IsAddon() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool IsBuilding() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual const IGameRace* Race() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int SupplyRequired() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int GasPrice() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int MineralsPrice() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool CanAttack() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool IsWorker() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool CanProduce() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool CanBuildAddOn() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool MaxHitpoints() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool BuildTime() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool IsResourceDepot() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int SupplyProvided() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int TileWidth() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int TileHeight() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool IsMineralsField() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool IsGasField() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int SightRange() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int DimensionLeft() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual int DimensionUp() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool IsRefinery() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual GameWeaponType GroundWeapon() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual GameWeaponType AirWeapon() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual const char* ToString() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

private:
    BWAPI::UnitType m_type;
    EntityClassType m_engineId;
};

//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapLastGameError(_In_ TID unitId) const
{
    Error lastErr = Broodwar->getLastError();
    BWAPI::Position unitPos = Broodwar->getUnit(unitId)->getPosition();

    Broodwar->registerEvent([unitPos, lastErr](Game*){ Broodwar->drawTextMap(unitPos, "%c%s", Text::Red, lastErr.c_str()); },   // action
        nullptr,    // condition
        Broodwar->getLatencyFrames());  // frames to run
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawUnitBuildBox(_In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const
{
    TilePosition tilePos(pos.X / TILE_SIZE, pos.Y / TILE_SIZE);
    auto type = UnitTypes::allUnitTypes()[pUnitType->GameId()];

    // Register an event that draws the target build location
    Broodwar->registerEvent([tilePos, type](Game*) {
        Broodwar->drawBoxMap(BWAPI::Position(tilePos),
            BWAPI::Position(tilePos + type.tileSize()),
            Colors::Blue); },
            nullptr,  // condition
            type.buildTime() + 100);  // frames to run
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::MapDebugDraw() const
{
    for (auto itr = Broodwar->getAllRegions().begin();
        itr != Broodwar->getAllRegions().end(); ++itr)
    {
        auto& regionR = (*itr);
        Broodwar->drawBoxMap(
            regionR->getBoundsLeft(),
            regionR->getBoundsTop(),
            regionR->getBoundsRight(),
            regionR->getBoundsBottom(),
            (regionR->isAccessible() ? BWAPI::Colors::Yellow : BWAPI::Colors::Grey),
            false);

        Broodwar->drawTextMap(regionR->getCenter(), "ID:%d RGRP-ID:%d", regionR->getID(), regionR->getRegionGroupID());

        for (auto nItr = regionR->getNeighbors().begin();
            nItr != regionR->getNeighbors().end(); ++itr)
        {
            auto& nRegionR = (*nItr);
            if (nRegionR->isAccessible())
                Broodwar->drawLineMap(regionR->getCenter(), nRegionR->getCenter(), BWAPI::Colors::Green);
        }
    }
}
#ifndef BWAPITYPES_H
#define BWAPITYPES_H

#include "RtsAiEngine.h"
#include "BWAPI.h"
#include "BwapiGame.h"

namespace IStrategizer
{
    class BwapiUnitType : public IGameUnitType
    {
    public:
        BwapiUnitType(BWAPI::UnitType type) : m_type(type)
        {

        }

        BWAPI::UnitType GetBwapiUnitType() const { return m_type; }

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
            
            return g_BwapiUnitTypes[m_type.whatBuilds().first.getID()];
        }

        virtual const IGameTechType* RequiredTech() const
        {
            return g_BwapiTechTypes[m_type.requiredTech().getID()];
        }

        virtual GameUnitTypeToCountListPtr RequiredUnits() const
        {
            const std::map<BWAPI::UnitType, int>& requiredUnits = m_type.requiredUnits();
            auto pArray = GameUnitTypeToCountListPtr(new GameUnitTypeToCountList(requiredUnits.size()));
            int count = 0;
            for each(auto item in requiredUnits)
            {
                Pair<const IGameUnitType*, int> itemPair;
                itemPair.Key = g_BwapiUnitTypes[item.first];
                itemPair.Value = item.second;
                pArray->At(count++) = itemPair;
            }

            return pArray;
        }

        virtual bool IsAddon() const
        {
            return m_type.isAddon();
        }

        virtual bool IsBuilding() const
        {
            return m_type.isBuilding();
        }

        virtual const IGameRace* Race() const
        {
            return g_BwapiRaces[m_type.getRace().getID()];
        }

        virtual int SupplyRequired() const
        {
            return m_type.supplyRequired();
        }

        virtual int GasPrice() const
        {
            return m_type.gasPrice();
        }

        virtual int MineralsPrice() const
        {
            return m_type.mineralPrice();
        }

        virtual bool CanAttack() const
        {
            return m_type.canAttack();
        }

        virtual bool IsWorker() const
        {
            return m_type.isWorker();
        }

        virtual bool CanProduce() const
        {
            return m_type.canProduce();
        }

        virtual bool CanBuildAddOn() const
        {
            return m_type.canBuildAddon();
        }

        virtual int MaxHitpoints() const
        {
            return m_type.maxHitPoints();
        }

        virtual int BuildTime() const
        {
            return m_type.buildTime();
        }

        virtual bool IsResourceDepot() const
        {
            return m_type.isResourceDepot();
        }

        virtual int SupplyProvided() const
        {
            return m_type.supplyProvided();
        }

        virtual int TileWidth() const
        {
            return m_type.tileWidth();
        }

        virtual int TileHeight() const
        {
            return m_type.tileHeight();
        }

        virtual bool IsMineralsField() const
        {
            return m_type.isMineralField();
        }

        virtual bool IsGasField() const
        {
            return (m_type.getID() == BWAPI::UnitTypes::Resource_Vespene_Geyser.getID());
        }

        virtual int SightRange() const
        {
            return m_type.sightRange();
        }

        virtual int DimensionLeft() const
        {
            return m_type.dimensionLeft();
        }

        virtual int DimensionUp() const
        {
            return m_type.dimensionUp();
        }

        virtual bool IsRefinery() const
        {
            return m_type.isRefinery();
        }

        virtual GameWeaponType GroundWeapon() const
        {
            GameWeaponType gameWeaponType;
            gameWeaponType.DamageAmount = m_type.groundWeapon().damageAmount();
            gameWeaponType.MaxRange = m_type.groundWeapon().maxRange();

            return gameWeaponType;
        }

        virtual GameWeaponType AirWeapon() const
        {
            GameWeaponType gameWeaponType;
            gameWeaponType.DamageAmount = m_type.airWeapon().damageAmount();
            gameWeaponType.MaxRange = m_type.airWeapon().maxRange();

            return gameWeaponType;
        }

        virtual const char* ToString() const
        {
            return m_type.c_str();
        }

    private:
        BWAPI::UnitType m_type;
    };

    class BwapiUpgradeType : public IGameUpgradeType
    {
    public:
        BwapiUpgradeType(BWAPI::UpgradeType type) : m_type(type)
        {

        }

        BWAPI::UpgradeType GetBwapiUpgradeType() const { return m_type; }

        virtual TID GameId() const
        {
            return m_type.getID();
        }

        virtual ResearchType EngineId() const
        {
            return (ResearchType)((int)RESEARCH_START + m_type.getID());
        }

        virtual int GasPrice() const
        {
            return m_type.gasPrice();
        }

        virtual int MineralsPrice() const
        {
            return m_type.mineralPrice();
        }

        virtual const IGameUnitType* WhatsRequired() const
        {
            return g_BwapiUnitTypes[m_type.whatsRequired().getID()];
        }

        virtual const IGameUnitType* WhatUpgrades() const
        {
            return g_BwapiUnitTypes[m_type.whatUpgrades().getID()];
        }

    private:
        BWAPI::UpgradeType m_type;

    };

    class BwapiRace : public IGameRace
    {
    public:
        BwapiRace(BWAPI::Race race) : m_race(race)
        {

        }

        BWAPI::Race GetBwapiRace() const { return m_race; }

        virtual TID GameId() const
        {
            return m_race.getID();
        }

        virtual const IGameUnitType* WorkerType() const
        {
            return g_BwapiUnitTypes[m_race.getWorker().getID()];
        }

        virtual const IGameUnitType* BaseType() const
        {
            return g_BwapiUnitTypes[m_race.getCenter().getID()];
        }

        virtual const IGameUnitType* SupplyProvider() const
        {
            return g_BwapiUnitTypes[m_race.getSupplyProvider().getID()];
        }

        virtual const IGameUnitType* GasProvider() const
        {
            return g_BwapiUnitTypes[m_race.getRefinery().getID()];
        }

        virtual const IGameUnitType* MineralsProvider() const
        {
            return g_BwapiUnitTypes[BWAPI::UnitTypes::Resource_Mineral_Field.getID()];
        }

    private:
        BWAPI::Race m_race;
    };

    class BwapiTechType : public IGameTechType
    {
    public:
        BwapiTechType(BWAPI::TechType type) : m_type(type)
        {

        }

        BWAPI::TechType GetBwapiTechType() const { return m_type; }

        virtual TID GameId() const
        {
            return m_type.getID();
        }

        virtual ResearchType EngineId() const
        {
            return (ResearchType)((int)RESEARCH_START + TechOffset + m_type.getID());
        }

        virtual int GasPrice() const
        {
            return m_type.gasPrice();
        }

        virtual int MineralsPrice() const
        {
            return m_type.mineralPrice();
        }

        virtual const IGameUnitType* WhatResearches() const
        {
            return g_BwapiUnitTypes[m_type.whatResearches().getID()];
        }

    private:
        static const int TechOffset = 127;

        BWAPI::TechType m_type;
    };
}
#endif // BWAPITYPES_H
#ifndef BWAPIUNITTYPE_H
#define BWAPIUNITTYPE_H

#include "RtsAiEngine.h"
#include "BWAPI.h"

namespace IStrategizer
{
    class BwapiRace : public IGameRace
    {
    public:
        BwapiRace(BWAPI::Race race) : m_race(race)
        {

        }

        virtual TID GameId() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        virtual const IGameUnitType* WorkerType() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        virtual const IGameUnitType* BaseType() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        virtual const IGameUnitType* SupplyProvider() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        virtual const IGameUnitType* GasProvider() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        virtual const IGameUnitType* MineralsProvider() const
        {
            throw std::logic_error("The method or operation is not implemented.");
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

        virtual TID GameId() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        virtual ResearchType EngineId() const
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

        virtual const IGameUnitType* WhatsRequired() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        virtual const IGameUnitType* WhatResearches() const
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    private:
        BWAPI::TechType m_type;
    };

    class BwapiUnitType : public IGameUnitType
    {
    public:
        BwapiUnitType(BWAPI::UnitType type) : m_type(type)
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
            return new BwapiUnitType(m_type.whatBuilds().first);
        }

        virtual const IGameTechType* RequiredTech() const
        {
            return new BwapiTechType(m_type.requiredTech());
        }

        virtual GameUnitTypeToCountList RequiredUnits() const
        {
            const std::map<BWAPI::UnitType, int>& requiredUnits = m_type.requiredUnits();
            ArrayList<Pair<const IGameUnitType*, int>>* arrayList = new ArrayList<Pair<const IGameUnitType*, int>>(requiredUnits.size());
            GameUnitTypeToCountList result(arrayList);
            int count = 0;
            for each(auto item in requiredUnits)
            {
                Pair<const IGameUnitType*, int> itemPair;
                itemPair.Key = new BwapiUnitType(item.first);
                itemPair.Value = item.second;
                result->At(count++) = itemPair;
            }

            return result;
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
            return new BwapiRace(m_type.getRace());
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
            return (m_type.getID() == BWAPI::UnitTypes::Resource_Vespene_Geyser.getID())
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
}
#endif // BWAPIUNITTYPE_H
#ifndef STARCRAFTRACE_H
#define STARCRAFTRACE_H

#include "GameRace.h"
#include "BWAPI.h"

namespace IStrategizer
{
    class StarCraftRace : public GameRace
    {
    public:
        StarCraftRace(BWAPI::Race race) :
            m_race(race) {}
        EntityClassType GetWorkerType() const;
        EntityClassType GetBaseType() const;
        EntityClassType GetResourceSource(ResourceType p_type) const;
        float GetResourceConsumbtionRatePerWorker(ResourceType p_id) const;
        int BaseSupplyAmount() const;
        int SupplyBuildingSupplyAmount() const;
        const std::string& ToString() const;

    private:
        static const float MineralsPerWorkerPerFrame;
        static const float GasPerWorkerPerFrame;
        BWAPI::Race m_race;
    };
}
#endif // STARCRAFTRACE_H
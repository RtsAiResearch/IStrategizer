#ifndef STARCRAFTRACE_H
#define STARCRAFTRACE_H

#include "GameRace.h"
#include "BWAPI.h"

namespace IStrategizer
{
    class StarCraftRace : public GameRace
    {
    public:
        StarCraftRace() : m_pPlayer(nullptr) {}
        StarCraftRace(BWAPI::Player pPlayer);
        EntityClassType GetWorkerType() const { return m_workerTypeId; }
        EntityClassType GetBaseType() const { return m_baseTypeId; }
        EntityClassType GetResourceSource(ResourceType p_type) const;
        float GetResourceConsumbtionRatePerWorker(ResourceType p_id) const;
        int BaseSupplyAmount() const;
        int SupplyBuildingSupplyAmount() const;
        std::string Name() const;

    private:
        static const float MineralsPerWorkerPerFrame;
        static const float GasPerWorkerPerFrame;

        EntityClassType m_workerTypeId;
        EntityClassType m_baseTypeId;
        BWAPI::Player m_pPlayer;
    };
}
#endif // STARCRAFTRACE_H
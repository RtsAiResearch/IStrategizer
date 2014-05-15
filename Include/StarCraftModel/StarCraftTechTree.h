#ifndef STARCRAFTTECHTREE_H
#define STARCRAFTTECHTREE_H

#ifndef GAMETECHTREE_H
#include "GameTechTree.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
{
    enum PlayerType;
    enum ResearchType;
    enum EntityClassType;

    class StarCraftTechTree : public GameTechTree
    {
    public:
        StarCraftTechTree() : m_pPlayer(nullptr) {}
        StarCraftTechTree(BWAPI::Player pPlayer);

        bool ResearchAvailable(ResearchType p_researchId) const;
        bool ResearchDone(ResearchType p_researchId) const ;
        EntityClassType TireBaseBuilding(BaseType p_tireId) const;
        EntityClassType SourceEntity(int p_typeOrResearchId) const;
        void GetRequirements(int p_typeOrResearchId, std::vector<ResearchType>& p_researches, std::map<EntityClassType, unsigned>& p_buildings);
        EntityClassType GetWorkerType() const { return m_workerTypeId; }
        EntityClassType GetBuilderType(EntityClassType p_buildingType) const;
        EntityClassType GetBaseType() const { return m_baseTypeId; }
        EntityClassType GetResourceSource(ResourceType p_type) const;
        float GetResourceConsumbtionRatePerWorker(ResourceType p_id) const;
        int BaseSupplyAmount() const;
        int SupplyBuildingSupplyAmount() const;

    protected:
        typedef std::vector<EntityClassType> Entities;
        typedef std::vector<ResearchType> Researches;
        typedef std::pair<Entities, Researches> Dependency;

        BWAPI::Player m_pPlayer;
        EntityClassType m_workerTypeId;
        EntityClassType m_baseTypeId;

        static const float MineralsPerWorkerPerFrame;
        static const float GasPerWorkerPerFrame;
    };  
}
#endif // STARCRAFTTECHTREE_H


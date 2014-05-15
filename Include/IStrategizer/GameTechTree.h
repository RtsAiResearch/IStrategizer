#ifndef GAMETECHTREE_H
#define GAMETECHTREE_H

#include <vector>
#include <map>

namespace IStrategizer
{
    enum ResearchType;
    enum EntityClassType;
    enum BaseType;
    enum ResourceType;

    class GameTechTree
    {
    public:
        virtual ~GameTechTree() {}

        virtual bool            ResearchDone(ResearchType p_researchId) const = 0;
        virtual bool            ResearchAvailable(ResearchType p_researchId) const = 0;
        virtual EntityClassType TireBaseBuilding(BaseType p_tireId) const = 0;
        virtual EntityClassType SourceEntity(int p_typeOrResearchId) const = 0;
        virtual void            GetRequirements(int p_typeOrResearchId, std::vector<ResearchType>& p_researches, std::map<EntityClassType, unsigned>& p_buildings) = 0;
        virtual EntityClassType GetWorkerType() const = 0;
        virtual EntityClassType GetBuilderType(EntityClassType p_buildingType) const = 0;
        virtual EntityClassType GetBaseType() const = 0;
        virtual EntityClassType GetResourceSource(ResourceType p_type) const = 0;
        virtual float GetResourceConsumbtionRatePerWorker(ResourceType p_id) const = 0;
        virtual int BaseSupplyAmount() const = 0;
        virtual int SupplyBuildingSupplyAmount() const = 0;
    };
}

#endif // GAMETECHTREE_H

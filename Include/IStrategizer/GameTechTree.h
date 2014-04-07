#ifndef GAMETECHTREE_H
#define GAMETECHTREE_H

#include <vector>
#include <map>
#include "IClonable.h"

namespace IStrategizer
{
    enum ResearchType;
    enum EntityClassType;
    enum BaseType;

    class GameTechTree : public IClonable
    {
    public:
        virtual ~GameTechTree() {}

        virtual bool            ResearchDone(ResearchType p_researchId) const = 0;
        virtual bool            ResearchAvailable(ResearchType p_researchId) const = 0;
        virtual EntityClassType TireBaseBuilding(BaseType p_tireId) const = 0;
        virtual EntityClassType SourceEntity(int p_typeOrResearchId) const = 0;
        virtual void            GetRequirements(int p_typeOrResearchId, std::vector<ResearchType>& p_researches, std::map<EntityClassType, unsigned>& p_buildings) = 0;
        virtual void            GetDependents(int p_typeOrResearchId, std::vector<ResearchType>& p_researches, std::vector<EntityClassType>& p_entityTypes) = 0;
        virtual EntityClassType GetWorkerType() = 0;
        virtual EntityClassType GetBaseType() = 0;
        void Copy(IClonable* pDest) {}
    };
}

#endif // GAMETECHTREE_H

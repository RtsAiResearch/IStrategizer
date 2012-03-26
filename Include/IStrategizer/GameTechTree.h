#ifndef GAMETECHTREE_H
#define GAMETECHTREE_H

#include <vector>
#include <map>
using namespace std;

namespace MetaData
{
    enum ResearchType;
    enum EntityClassType;
    enum BaseType;
}

namespace IStrategizer
{
    using namespace MetaData;
    class GameTechTree
    {
    public:
		virtual ~GameTechTree() {}

		virtual bool            ResearchDone(ResearchType p_researchId) const = 0;
		virtual bool            ResearchAvailable(ResearchType p_researchId) const = 0;
        virtual EntityClassType TireBaseBuilding(BaseType p_tireId) const = 0;
        virtual EntityClassType SourceEntity(int p_typeOrResearchId) const = 0;
        virtual void            GetRequirements(int p_typeOrResearchId, vector<ResearchType>& p_researches, map<EntityClassType, unsigned>& p_buildings) = 0;
        virtual void            GetDependents(int p_typeOrResearchId, vector<ResearchType>& p_researches, vector<EntityClassType>& p_entityTypes) = 0;
    };
}

#endif // GAMETECHTREE_H
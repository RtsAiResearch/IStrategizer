#ifndef STARCRAFTTECHTREE_H
#define STARCRAFTTECHTREE_H

#ifndef GAMETECHTREE_H
#include "GameTechTree.h"
#endif
#ifndef MAPEX_H
#include "MapEx.h"
#endif

namespace BWAPI
{
	class Player;
}

namespace MetaData
{
    enum PlayerType;
    enum ResearchType;
    enum EntityClassType;
}

namespace StarCraftModel
{
	using namespace MetaData;
	using namespace std;
	using namespace BWAPI;
	using namespace DataStructure;

	class StarCraftTechTree : public IStrategizer::GameTechTree
    {
		typedef std::vector<MetaData::EntityClassType> Entities;
		typedef std::vector<MetaData::ResearchType> Researches;
		typedef std::pair<Entities, Researches> Dependency;

		Player* m_player;

    public:
		StarCraftTechTree(Player* p_player) : m_player(p_player) {}
		bool			ResearchAvailable(ResearchType p_researchId) const;
        bool			ResearchDone(ResearchType p_researchId) const ;
        EntityClassType TireBaseBuilding(BaseType p_tireId) const;
        EntityClassType SourceEntity(int p_typeOrResearchId) const;
		void            GetRequirements(int p_typeOrResearchId, vector<ResearchType>& p_researches, map<EntityClassType, unsigned>& p_buildings);
        void            GetDependents(int p_typeOrResearchId, vector<ResearchType>& p_researches, vector<EntityClassType>& p_entityTypes);
    };  
}
#endif // STARCRAFTTECHTREE_H
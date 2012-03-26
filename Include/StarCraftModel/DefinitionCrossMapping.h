#ifndef DEFINITIONCROSSMAPPING_H
#define DEFINITIONCROSSMAPPING_H

#include <vector>
#include <string>

#include "CrossMap.h"
#include "EngineData.h"

using namespace DataStructure;

typedef std::string TName;

extern const int TechIdOffset;

class DefinitionCrossMapping
{
private:
	DefinitionCrossMapping();
	void InitEntities();
	void InitUpgrades();
	void InitTeches();
	void InitPlayers();
	void InitEntityIdents();
	void InitUpgradeIdents();
	void InitTechIdents();
	bool m_initialized;

public:
	bool ExportGameIds(std::string p_exportPath);
	bool ExportAllIds(std::string p_exportPath);
	void EntityTypes(std::vector<MetaData::EntityClassType>& p_entityTypes);
	void ResearchTypes(std::vector<MetaData::ResearchType>& p_researchTypes);
	void Init();

	inline static DefinitionCrossMapping& Instance() { static DefinitionCrossMapping m_instance; return m_instance; }

	CrossMap<TID, MetaData::EntityClassType>	EntityMapping;
	CrossMap<TID, MetaData::ResearchType>		UpgradeMapping;
	CrossMap<TID, MetaData::ResearchType>		TechMapping;
	CrossMap<TID, MetaData::PlayerType>			PlayerMapping;
	CrossMap<TID, TName>						EntityIdentMapping;		
	CrossMap<TID, TName>						UpgradeIdentMapping;
	CrossMap<TID, TName>						TechIdentMapping;
};

#define g_Database DefinitionCrossMapping::Instance()

#endif	// DEFINITIONCROSSMAPPING_H
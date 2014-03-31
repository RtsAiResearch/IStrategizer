#ifndef DEFINITIONCROSSMAPPING_H
#define DEFINITIONCROSSMAPPING_H

#include <vector>
#include <string>

#include "CrossMap.h"
#include "EngineData.h"

using namespace IStrategizer;

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
	void InitResources();
	void InitActions();
    bool m_initialized;

public:
    bool ExportGameIds(std::string p_exportPath);
    bool ExportAllIds(std::string p_exportPath);
    void EntityTypes(std::vector<IStrategizer::EntityClassType>& p_entityTypes);
    void ResearchTypes(std::vector<IStrategizer::ResearchType>& p_researchTypes);
    void Init();

    inline static DefinitionCrossMapping& Instance() { static DefinitionCrossMapping m_instance; return m_instance; }
    CrossMap<TID, IStrategizer::EntityClassType> EntityMapping;
    CrossMap<TID, IStrategizer::ResearchType> UpgradeMapping;
    CrossMap<TID, IStrategizer::ResearchType> TechMapping;
    CrossMap<TID, IStrategizer::PlayerType> PlayerMapping;
    CrossMap<TID, IStrategizer::ActionType> ActionMapping;
	CrossMap<TID, IStrategizer::ResourceType> ResourceMapping;
    CrossMap<TID, TName> EntityIdentMapping; 
    CrossMap<TID, TName> UpgradeIdentMapping;
    CrossMap<TID, TName> TechIdentMapping;
};

#define g_Database DefinitionCrossMapping::Instance()

#endif // DEFINITIONCROSSMAPPING_H

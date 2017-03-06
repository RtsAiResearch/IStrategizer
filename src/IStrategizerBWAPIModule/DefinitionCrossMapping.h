#ifndef DEFINITIONCROSSMAPPING_H
#define DEFINITIONCROSSMAPPING_H

#include <vector>
#include <string>

#include "CrossMap.h"
#include "MetaData.h"

using namespace IStrategizer;

typedef std::string TName;

extern const int TechIdOffset;

typedef std::string TAction;
typedef std::string TEntity;
typedef std::string TUpgrade;
typedef int TPlayer;

class DefinitionCrossMapping
{
private:
    DefinitionCrossMapping();
    void InitEntityIdents();
    void InitUpgradeIdents();
    void InitTechIdents();
    void InitActions();
    bool m_initialized;

public:
    void Init();

    inline static DefinitionCrossMapping& Instance() { static DefinitionCrossMapping m_instance; return m_instance; }
    CrossMap<TID, IStrategizer::ActionType> ActionMapping;
    CrossMap<TID, TName> EntityIdentMapping; 
    CrossMap<TID, TName> UpgradeIdentMapping;
    CrossMap<TID, TName> TechIdentMapping;
};

#define g_Database DefinitionCrossMapping::Instance()

#endif // DEFINITIONCROSSMAPPING_H

#ifndef STARCRAFTTYPE_H
#define STARCRAFTTYPE_H

#ifndef GAMETYPE_H
#include "GameType.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
{
    class StarCraftType : public GameType
    {
    public:
        StarCraftType(BWAPI::UnitType p_entityType) : m_type(p_entityType) {}
        void Init();
        EntityClassType GetBuilderType() const;
        EntityClassType SourceEntity() const;
        void GetRequirements(std::vector<ResearchType>& researches, std::map<EntityClassType, unsigned>& buildings) const;

    private:
        BWAPI::UnitType m_type;
    };
}

#endif // STARCRAFTTYPE_H

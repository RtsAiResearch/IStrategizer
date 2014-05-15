#ifndef STARCRAFTTYPE_H
#define STARCRAFTTYPE_H

#ifndef GAMETYPE_H
#include "GameType.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
{
    class StarCraftType : public IStrategizer::GameType
    {
    public:
        StarCraftType(BWAPI::UnitType p_entityType) : m_type(p_entityType) {}
        void Init();

    private:
        BWAPI::UnitType m_type;
    };
}

#endif // STARCRAFTTYPE_H

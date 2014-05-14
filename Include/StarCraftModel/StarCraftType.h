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
        BWAPI::UnitType m_type;

    protected:
        void    InitializeAux();

    public:
        StarCraftType(BWAPI::UnitType p_entityType) : m_type(p_entityType) {}
    };
}

#endif // STARCRAFTTYPE_H

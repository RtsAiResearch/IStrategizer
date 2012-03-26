#ifndef STARCRAFTTYPE_H
#define STARCRAFTTYPE_H

#ifndef GAMETYPE_H
#include "GameType.h"
#endif

#include "bwapi/UnitType.h"

namespace StarCraftModel
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
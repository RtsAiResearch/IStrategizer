#ifndef BWAPIWRAPPER_H
#define BWAPIWRAPPER_H

#include "RtsAiEngine.h"

namespace IStrategizer
{
    class BwapiWrapper : public IRtsGame
    {
    public:
        void DebugDrawMapLastGameError(_In_ TID unitId) const;
        void DebugDrawUnitBuildBox(_In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const;
        void MapDebugDraw() const;
    };
}

#endif // BWAPIWRAPPER_H
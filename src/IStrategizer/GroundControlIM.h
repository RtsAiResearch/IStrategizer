#ifndef GROUNDCONTROLIM_H
#define GROUNDCONTROLIM_H

#include "InfluenceMap.h"

namespace IStrategizer
{
    class GroundControlIM : public InfluenceMap
    {
    public:
        GroundControlIM(IMType p_typeId) : InfluenceMap(p_typeId) {}
        void Update();
        TInfluence GetCellInfluenceFromWorldPosition(const Vector2 p_worldPosition);

    private:
        void GetInfluence(RegObjEntry *p_pObjEntry, int &p_effectiveDistance, int &p_maxDistance, TCell &p_initValue);
    };
}

#endif // GROUNDCONTROLIM_H

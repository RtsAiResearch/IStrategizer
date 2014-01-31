///> [Serializable]
#ifndef ENTITYCLASSNEARAREA_H
#define ENTITYCLASSNEARAREA_H

#include "ConditionEx.h"
#ifndef CELLFEATURE_H
#include "CellFeature.h"
#endif


#define NEGLECTED_AMOUNT -1

namespace IStrategizer
{
    ///> class=EntityClassNearArea
    ///> parent=ConditionEx
    class EntityClassNearArea : public ConditionEx
    {
        OBJECT_SERIALIZABLE(EntityClassNearArea);

    public:
        EntityClassNearArea() {}
        EntityClassNearArea(PlayerType p_player, EntityClassType p_entityType, CellFeature* cellFeature, int howFar, int p_amount =  NEGLECTED_AMOUNT);

        bool Evaluate(RtsGame& pRtsGame);
        bool Consume(int p_amount) { return true; }
    };
}

#endif // ENTITYCLASSNEARAREA_H

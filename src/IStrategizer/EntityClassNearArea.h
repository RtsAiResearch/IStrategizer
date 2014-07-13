///> [Serializable]
#ifndef ENTITYCLASSNEARAREA_H
#define ENTITYCLASSNEARAREA_H


#include "EngineData.h"
#include "ConditionEx.h"
#ifndef CELLFEATURE_H
#include "CellFeature.h"
#endif


namespace IStrategizer
{
    ///> class=EntityClassNearArea
    ///> parent=ConditionEx
    class EntityClassNearArea : public ConditionEx
    {
		OBJECT_SERIALIZABLE_P(EntityClassNearArea, ConditionEx);

    public:
        EntityClassNearArea() {}
        EntityClassNearArea(PlayerType p_player, EntityClassType p_entityType, CellFeature* cellFeature, int howFar, int p_amount =  DONT_CARE);

        bool Evaluate(RtsGame& game);
        bool Consume(int p_amount) { return true; }
    };
}

#endif // ENTITYCLASSNEARAREA_H

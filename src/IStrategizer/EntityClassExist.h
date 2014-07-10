///> [Serializable]
#ifndef ENTITYCLASSEXIST_H
#define ENTITYCLASSEXIST_H

#include "ConditionEx.h"

#include <vector>
#include <map>

namespace IStrategizer
{
    ///> class=EntityClassExist
    ///> parent=ConditionEx
    class EntityClassExist : public ConditionEx
    {
		OBJECT_SERIALIZABLE_P(EntityClassExist, ConditionEx);

    public:
        EntityClassExist() {}
        EntityClassExist(PlayerType p_player);
        EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId);
        EntityClassExist(PlayerType p_player, int p_unitClassId, int p_amount);
        bool Evaluate(RtsGame& game);
        bool Consume(int p_amount);
    };
}

#endif // ENTITYCLASSEXIST_H

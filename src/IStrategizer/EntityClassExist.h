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
        EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, ObjectStateType state = OBJSTATE_END, bool checkFree = false);
        EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, ObjectStateType state = OBJSTATE_END, bool checkFree = false);
        bool Evaluate(RtsGame& game);
        bool Consume(int p_amount);

    private:
        bool m_checkFree;
    };
}

#endif // ENTITYCLASSEXIST_H

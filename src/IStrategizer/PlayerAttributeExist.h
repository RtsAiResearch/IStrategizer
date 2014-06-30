///> [Serializable]
#ifndef PLAYERATTRIBUTEEXIST_H
#define PLAYERATTRIBUTEEXIST_H

#include "ConditionEx.h"

namespace IStrategizer
{
    ///> class=PlayerAttributeExist
    ///> parent=ConditionEx
    class PlayerAttributeExist : public ConditionEx
    {
        OBJECT_SERIALIZABLE(PlayerAttributeExist);

    public:
        PlayerAttributeExist() {}
        PlayerAttributeExist(PlayerType playerType, PlayerAttribute p_attribute, int amount);
        bool Evaluate(RtsGame& game);
        bool Consume(int p_amount);

    private:
        PlayerAttribute m_attribute;
    };
}

#endif // RESEARCHDONE_H

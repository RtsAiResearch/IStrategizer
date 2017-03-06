///> [Serializable]
#ifndef TRAINEREXIST_H
#define TRAINEREXIST_H

#include "ConditionEx.h"

namespace IStrategizer
{
    ///> class=TrainerExist 
    ///> parent=ConditionEx
    class TrainerExist : public ConditionEx
    {
        OBJECT_SERIALIZABLE_P(TrainerExist, ConditionEx);

    public:
        TrainerExist() {}
        TrainerExist(EntityClassType buildingTypeId);
        bool Evaluate(RtsGame& game);
        bool Consume(int p_amount);
    };
}

#endif // TRAINEREXIST_H

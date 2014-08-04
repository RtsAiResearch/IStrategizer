///> [Serializable]
#ifndef BUILDEREXIST_H
#define BUILDEREXIST_H

#include "ConditionEx.h"

namespace IStrategizer
{
    ///> class=BuilderExist 
    ///> parent=ConditionEx
    class BuilderExist : public ConditionEx
    {
        OBJECT_SERIALIZABLE_P(BuilderExist, ConditionEx);

    public:
        BuilderExist() {}
        BuilderExist(EntityClassType buildingTypeId);
        bool Evaluate(RtsGame& game);
        bool Consume(int p_amount);
    };
}

#endif // BUILDEREXIST_H

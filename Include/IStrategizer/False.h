///> [Serializable]
#ifndef FALSE_H
#define FALSE_H

#ifndef CONDITIONEX_H
#include "ConditionEx.h"
#endif

namespace IStrategizer
{
    ///> class=False
    ///> parent=ConditionEx
    class False : public ConditionEx
    {
        OBJECT_SERIALIZABLE(False);

    public:
        False() : ConditionEx(PLAYER_Self, CONDEX_False) {}
        bool Evaluate(RtsGame* pRtsGame) { return false; }
        bool Consume(int p_amount) { return true; }
    };
}
#endif // FALSE_H

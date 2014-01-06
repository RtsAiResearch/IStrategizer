///> [Serializable]
#ifndef TRUE_H
#define TRUE_H

#ifndef CONDITIONEX_H
#include "ConditionEx.h"
#endif

namespace IStrategizer
{
    ///> class=True
    ///> parent=ConditionEx
    class True : public ConditionEx
    {
        public:
            True() : ConditionEx(PLAYER_Self, CONDEX_False) {}
            bool Evaluate(RtsGame* pRtsGame) { return true; }
            bool Consume(int p_amount) { return true; }
    };
}

#endif // TRUE_H

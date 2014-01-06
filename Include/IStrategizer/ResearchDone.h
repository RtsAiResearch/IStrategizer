///> [Serializable]
#ifndef RESEARCHDONE_H
#define RESEARCHDONE_H

#include "ConditionEx.h"

namespace IStrategizer
{
    ///> class=ResearchDone
    ///> parent=ConditionEx
    class ResearchDone : public ConditionEx
    {
        OBJECT_SERIALIZABLE(ResearchDone);

    public:
        ResearchDone() {}
        ResearchDone(PlayerType p_player, ResearchType p_researchId) : ConditionEx(p_player, CONDEX_ResearchDone) { _conditionParameters[PARAM_ResearchId] = p_researchId; }
        bool Evaluate(RtsGame* pRtsGame);
        bool Consume(int p_amount) { return true; }
    };
}

#endif // RESEARCHDONE_H

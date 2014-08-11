///> [Serializable]
#ifndef RESEARCHEREXIST_
#define RESEARCHEREXIST_

#include "ConditionEx.h"

namespace IStrategizer
{
    ///> class=ResearcherExist 
    ///> parent=ConditionEx
    class ResearcherExist : public ConditionEx
    {
        OBJECT_SERIALIZABLE_P(TrainerExist, ConditionEx);

    public:
        ResearcherExist() {}
        ResearcherExist(ResearchType researcgTypeId);
        bool Evaluate(RtsGame& game);
        bool Consume(int p_amount);
    };
}

#endif // RESEARCHEREXIST_

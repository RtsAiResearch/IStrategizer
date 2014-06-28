#include "ResearchDone.h"
#include "GamePlayer.h"

using namespace IStrategizer;

bool ResearchDone::Evaluate(RtsGame& game)
{
   _isSatisfied = game.GetPlayer((PlayerType)_conditionParameters[PARAM_PlayerId])->TechTree()->ResearchDone((ResearchType)_conditionParameters[PARAM_ResearchId]);

    _isEvaluated = true;

    return _isEvaluated && _isSatisfied;
}

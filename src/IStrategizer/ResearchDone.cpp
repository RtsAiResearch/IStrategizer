#include "ResearchDone.h"
#include "GamePlayer.h"

using namespace IStrategizer;

bool ResearchDone::Evaluate(RtsGame& game)
{
   _isSatisfied = game.GetPlayer((PlayerType)m_params[PARAM_PlayerId])->TechTree()->ResearchDone((ResearchType)m_params[PARAM_ResearchId]);

    _isEvaluated = true;

    return _isEvaluated && _isSatisfied;
}

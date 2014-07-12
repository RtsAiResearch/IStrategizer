///> [Serializable]
#ifndef CANDIDATEGATHEREREXIST_H
#define CANDIDATEGATHEREREXIST_H

#include "ConditionEx.h"

namespace IStrategizer
{
	///> class=CandidateGathererExist 
	///> parent=ConditionEx
	class CandidateGathererExist : public ConditionEx
	{
		OBJECT_SERIALIZABLE_P(CandidateGathererExist, ConditionEx);

	public:
		CandidateGathererExist() {}
		CandidateGathererExist(PlayerType player, ResourceType resourceType);
		bool Evaluate(RtsGame& game);
		bool Consume(int p_amount);
	};
}

#endif // CANDIDATEGATHEREREXIST_H

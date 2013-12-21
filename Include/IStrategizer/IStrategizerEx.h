#ifndef ISTRATEGIZEREX_H
#define ISTRATEGIZEREX_H

#include <vector>
#include "MetaData.h"
#include "MessagePumpObserver.h"
#include "WorldClock.h"

namespace IStrategizer
{
	class MessagePump;
	class TraceEx;
	class CaseBaseEx;
	class LearningFromHumanDemonstrationEx;
	class GameAPI;
	class OnlineCaseBasedPlannerEx;
	class RtsGame;

	struct IStrategizerParam
	{
		unsigned GrndCtrlIMUpdateInterval;
		unsigned OccupanceIMUpdateInterval;
		int BuildingDataIMCellSize;
		int GrndCtrlIMCellSize;
	};

	class IStrategizerEx : public MessagePumpObserver
	{
	private:
		OnlineCaseBasedPlannerEx*			_planner;
		LearningFromHumanDemonstrationEx*	_caseLearning;
		PlayerType							_self;
		PlayerType							_enemy;
		PhaseType							_phase;
		IStrategizerParam					_param;
		bool								_isFirstUpdate;
		WorldClock							_clock;

	public:
		IStrategizerEx(const IStrategizerParam &p_param, PhaseType p_phaseType, RtsGame* p_rtsGame);
		void	Update(unsigned p_gameCycle);
		void	OfflineLearning();
		const OnlineCaseBasedPlannerEx* Planner() const { return _planner; }
		void	NotifyMessegeSent(Message* p_message);
		const WorldClock& Clock() const { return _clock; }
		~IStrategizerEx();
	};
}

#endif	// ISTRATEGIZEREX_H
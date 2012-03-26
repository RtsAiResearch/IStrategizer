#ifndef ISTRATEGIZEREX_H
#define ISTRATEGIZEREX_H

#include <vector>
#include "MetaData.h"
#include "MessagePumpObserver.h"

using namespace std;
using namespace MetaData;

class MessagePump;
class TraceEx;
class CaseBaseEx;
class LearningFromHumanDemonstrationEx;
class GameAPI;

namespace OLCBP
{
    class OnlineCaseBasedPlannerEx;
}
namespace IStrategizer
{
    class RtsGame;
}

using namespace OLCBP;

struct IStrategizerParam
{
	int IMSysUpdateInterval;
	int BuildingDataIMCellSize;
	int GroundControlIMCellSize;
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

public:
            IStrategizerEx(const IStrategizerParam &p_param, PhaseType p_phaseType, IStrategizer::RtsGame* p_rtsGame);
	void	Update(unsigned long p_gameCycle);
	void	OfflineLearning();
	void	NotifyMessegeSent(Message* p_message);
            ~IStrategizerEx();
};

#endif	// ISTRATEGIZEREX_H
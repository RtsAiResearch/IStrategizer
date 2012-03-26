///> [Serializable]
#ifndef	GOALEX_H
#define GOALEX_H

#include <vector>
#include "PlanStepEx.h"
#include "EngineData.h"
#include "CompositeExpression.h"

using namespace std;
using namespace MetaData;

///> class=GoalEx
///> parent=PlanStepEx
class GoalEx : public PlanStepEx
{
private:
protected:
	inline	GoalEx(GoalType p_goalType) : PlanStepEx((int)p_goalType, ESTATE_END) {}
	inline	GoalEx(GoalType p_goalType, const PlanStepParameters& p_parameters) : PlanStepEx(p_goalType, ESTATE_END, p_parameters) {}
public:
	void	UpdateAux(unsigned p_cycles);
	void	Reset(unsigned p_cycles);
};
#endif	// GOALEX_H
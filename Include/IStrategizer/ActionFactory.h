#ifndef ACTIONFACTORY_H
#define ACTIONFACTORY_H

#include <vector>
#include "MetaData.h"

using namespace std;
using namespace MetaData;

class Action;
namespace IStrategizer
{
	class CellFeature;
}

class ActionFactory
{
private:
	ActionFactory() {}
public:
	static	ActionFactory& Instance() { static ActionFactory m_instance; return m_instance; }
	Action*	GetAction(ActionType p_actionType, PlanStepParameters& p_paramaters, IStrategizer::CellFeature *p_cellFeature = NULL, bool p_initConditions = true);
	Action*	GetAction(ActionType p_actionType, bool p_initConditions = true);
    Action* GetBuildAction( const vector<int>& p_paramaters );
	#define g_ActionFactory ActionFactory::Instance()
};

#endif // ACTIONFACTORY_H
#ifndef ACTIONFACTORY_H
#define ACTIONFACTORY_H

#include <vector>
#include "MetaData.h"

namespace IStrategizer
{
    class Action;
    class CellFeature;

    class ActionFactory
    {
    private:
        ActionFactory() {}
    public:
        static    ActionFactory& Instance() { static ActionFactory m_instance; return m_instance; }
        Action*    GetAction(ActionType p_actionType, const PlanStepParameters& p_paramaters, bool p_initConditions = true);
        Action*    GetAction(ActionType p_actionType, bool p_initConditions = true);
        Action* GetBuildAction(const std::vector<int>& p_paramaters);
    };

#define g_ActionFactory IStrategizer::ActionFactory::Instance()

}

#endif // ACTIONFACTORY_H
#ifndef ACTIONFACTORY_H
#define ACTIONFACTORY_H

#include <vector>
#include "MetaData.h"

using namespace std;

namespace IStrategizer
{
    class Action;
    class CellFeature;

    class ActionFactory
    {
    private:
        RtsGame* m_RtsGame;
        ActionFactory() {}
    public:
        static    ActionFactory& Instance();
        static    ActionFactory& Instance(RtsGame& p_RtsGame);
        Action*    GetAction(ActionType p_actionType, const PlanStepParameters& p_paramaters);
        Action*    GetAction(ActionType p_actionType);
        Action* GetBuildAction(const vector<int>& p_paramaters);
    };
}

#endif // ACTIONFACTORY_H

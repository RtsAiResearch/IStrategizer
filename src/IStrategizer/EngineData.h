///> [Serializable]
#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <unordered_set>
#include "MetaData.h"
#include "SVector.h"
#include "SMap.h"
#include "AdjListDigraph.h"

namespace IStrategizer
{
    enum BattleStateType
    {
        // State for targeting an enemy unit.
        Target,

        // State for performing attack action on the targeted unit.
        Attack,

        // State indicating that the battle is finished. Either all army entities or 
        // targeted enemy entities are destroyed
        Finished
    };

    class PlanStepEx;
    class ConditionEx;
    class GoalEx;

    typedef std::string TAction;
    typedef std::string TEntity;
    typedef std::string TUpgrade;

    typedef int TPlayer;
    typedef int TID;

    typedef std::unordered_set<TID> EntitySet;
    typedef std::vector<TID> EntityList;

    ///> alias=PlanStepParameters(map(pair(int,int)))
    typedef Serialization::SMap<ParameterType, int> PlanStepParameters;

    ///> alias=IOlcbpPlan(IDigraph(PlanStepEx*))
    typedef IDigraph<PlanStepEx*> IOlcbpPlan;

    ///> alias=ShallowFeaturesEx(vector(float))
    typedef Serialization::SVector<float> ShallowFeaturesEx;

    ///> alias=DeepFeaturesEx(vector(float))
    typedef Serialization::SVector<float> DeepFeaturesEx;
}

#define INVALID_TID -1
#define DONT_CARE -1

#endif // ENGINEDATA_H
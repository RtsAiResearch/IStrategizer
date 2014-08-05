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

        // State for performing naive attack action on the targeted unit.
        BasicAttack,

        // State for performing attack using tanks, where the tanks are going to use the
        // siege mode to destroy enemy units from far range and they will be guarded with
        // marines
        TankAttack,

        // State for deploying the army to a specific enemy target, where the units will stop
        // moving when the enemy target is in range
        Deploy,

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
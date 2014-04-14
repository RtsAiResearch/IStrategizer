///> [Serializable]
#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <vector>
#include <map>
#include "MetaData.h"

#ifndef SVECTOR_H
#include "SVector.h"
#endif

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

#include "AdjListDigraph.h"

namespace IStrategizer
{
    class PlanStepEx;
    class GameStateEx;
    class ConditionEx;
    class GoalEx;

    typedef std::string TAction;
    typedef std::string TEntity;
    typedef std::string TUpgrade;

    typedef int TPlayer;
    typedef int TID;

    ///> alias=PlanStepParameters(map(pair(int,int)))
    typedef Serialization::SMap<ParameterType, int> PlanStepParameters;

    ///> alias=IOlcbpPlan(IDigraph(PlanStepEx*))
    typedef IDigraph<PlanStepEx*> IOlcbpPlan;

    ///> alias=OlcbpPlan(AdjListDigraph(PlanStepEx*))
    typedef AdjListDigraph<PlanStepEx*> OlcbpPlan;

    ///> alias=ShallowFeaturesEx(vector(float))
    typedef Serialization::SVector<float> ShallowFeaturesEx;

    ///> alias=DeepFeaturesEx(vector(float))
    typedef Serialization::SVector<float> DeepFeaturesEx;

#define INVALID_TID -1
#define DONT_CARE -1
#define ExactSimilarity 100

    /********************************************************************* Header Annotations ************************************************************************/
#ifndef IN
#define IN __in
#endif

#ifndef OUT
#define OUT __out
#endif

#ifndef CheckReturn
#define CheckReturn __checkReturn
#endif
}

#endif // ENGINEDATA_H

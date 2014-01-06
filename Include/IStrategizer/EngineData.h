///> [Serializable]
#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <vector>
#include <map>
#include "MetaData.h"

#ifndef SVECTOR_H
#include "SVector.h"
#endif

#ifndef SMAP_H
#include "SMap.h"
#endif

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

namespace IStrategizer
{
    class PlanStepEx;
    class GameStateEx;
    class PlanGraph;
    class ConditionEx;
    class GoalEx;

    /**************************************************************** Engine Custom Definitions****************************************************************/
    typedef string TAction;
    typedef string TEntity;
    typedef string TUpgrade;
    //typedef CostType TCost;
    typedef int TPlayer;
    typedef int TID;

    /**************************************************************** Conditions Definitions****************************************************************/
#define ExactSimilarity 100

    /**************************************************************** Game State Definitions ****************************************************************/

    ///> alias=ShallowFeaturesEx(vector(float))
    typedef Serialization::SVector<float> ShallowFeaturesEx;
    ///> alias=DeepFeaturesEx(vector(float))
    typedef Serialization::SVector<float> DeepFeaturesEx;

    /*********************************************************** Learning From Human Demonstration Definitions ***********************************************************/


#define MAX_PLAYER_ENTITIES 90
#define INPUT_ACTION "input"
#define MOVE_ACTION "move"

    /********************************************************************* Static Paths **************************************************************************/

#define CASEBASE_PATH_EX "Casebase.txt"
#define LOG_PATH_EX "wc2\\logs\\CaseLearningLog.log"

    /********************************************************************* Misc **************************************************************************/

    ///> alias=PlanStepParameters(map(pair(int,int)))
    typedef Serialization::SMap<ParameterType, int> PlanStepParameters;
#define GOALS_WITH_PARAMS_COUNT 27
#define FARM_FOOD_SUPPLY 4
#define INVALID_TID -1
#define PARAM_DONT_CARE -1
#define ANY_ID -2
#define MAX_POPULATION -3
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

#endif // ENGINEDATA_H*******************************

///> [Serializable]
#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <vector>
#include <map>
#include "EngineMetaData.h"

#ifndef SVECTOR_H
	#include "SVector.h"
#endif

#ifndef SMAP_H
	#include "SMap.h"
#endif

#ifndef USEROBJECT_H
	#include "UserObject.h"
#endif
using namespace Serialization;

using namespace std;
using namespace EngineMetaData;

class PlanStepEx;
class GameStateEx;
class PlanGraph;
class ConditionEx;
class GoalEx;

/**************************************************************** Conditions Definitions****************************************************************/
#define ExactSimilarity 100

/**************************************************************** Game State Definitions ****************************************************************/

typedef SVector<double>									ShallowFeaturesEx;
typedef SVector<double>									DeepFeaturesEx;
typedef vector<vector<DeepFeatureType>>					SituationDeepFeaturesEx;

/*********************************************************** Learning From Human Demonstration Definitions ***********************************************************/

typedef SVector<int>								GoalSatisfactionEx;
typedef	SVector<PlanStepEx*>						SequentialPlan;

struct RawPlanEx : public UserObject
{
	GoalEx*			Goal;
	SequentialPlan	sPlan;

	RawPlanEx(){}
	RawPlanEx(GoalEx* p_goal, SequentialPlan p_sPlan): Goal(p_goal), sPlan(p_sPlan){}
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "RawPlanEx"; }
	int         TypeSize()  { return sizeof(RawPlanEx); }
    UserObject* Prototype() { return new RawPlanEx; }	
protected:
    void InitializeAddressesAux() { AddMemberAddress(2, &Goal, &sPlan); }
	//----------------------------------------------------------------------------------------------
};

struct RawCaseEx : public UserObject
{
	RawPlanEx		rawPlan;
	GameStateEx*	gameState;

	RawCaseEx(){}
	RawCaseEx(RawPlanEx p_rawPlan, GameStateEx*	p_gameState): rawPlan(p_rawPlan), gameState(p_gameState){}
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "RawCaseEx"; }
    int         TypeSize()  { return sizeof(RawCaseEx); }
    UserObject* Prototype() { return new RawCaseEx; }	
protected:
    void InitializeAddressesAux() { AddMemberAddress(2, &rawPlan, &gameState); }
    //----------------------------------------------------------------------------------------------
};

struct CookedPlan : public UserObject
{
	GoalEx*			Goal;
	GameStateEx*	gameState;
	PlanGraph*		pPlan;

	CookedPlan(){}
	CookedPlan(GoalEx* p_goal, PlanGraph* p_pPlan, GameStateEx* p_gameState): Goal(p_goal), pPlan(p_pPlan), gameState(p_gameState) {}
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "CookedPlan"; }
    int         TypeSize()  { return sizeof(CookedPlan); }
    UserObject* Prototype() { return new CookedPlan; }	
protected:
    void InitializeAddressesAux() { AddMemberAddress(3, &Goal, &gameState, &pPlan); }
	//----------------------------------------------------------------------------------------------
};

struct CookedCase : public UserObject
{
	RawCaseEx*	rawCase;
	PlanGraph*	dGraph;

	CookedCase(){}
	CookedCase(RawCaseEx* p_rawCase, PlanGraph* p_dGraph): rawCase(p_rawCase), dGraph(p_dGraph) {}
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "CookedCase"; }
    int         TypeSize()  { return sizeof(CookedCase); }
    UserObject* Prototype() { return new CookedCase; }	
protected:
    void InitializeAddressesAux() { AddMemberAddress(2, &rawCase, &dGraph); }
	//----------------------------------------------------------------------------------------------
};

#define MAX_PLAYER_ENTITIES						90
#define INPUT_ACTION							"input"
#define MOVE_ACTION								"move"

/********************************************************************* Static Paths **************************************************************************/

#define CASEBASE_PATH_EX			"Casebase.txt"
#define LOG_PATH_EX					"wc2\\logs\\CaseLearningLog.log"

/********************************************************************* Misc **************************************************************************/

///> alias=PlanStepParameters(map(pair(int,int)))
typedef Serialization::SMap<ParameterType, int>	PlanStepParameters;
#define	GOALS_WITH_PARAMS_COUNT	27
#define	FARM_FOOD_SUPPLY		4
/********************************************************************* Header Annotations ************************************************************************/
#ifndef IN
#define IN			__in
#endif

#ifndef OUT
#define OUT			__out
#endif

#ifndef CheckReturn
#define CheckReturn	__checkReturn
#endif


#endif	// ENGINEDATA_H*******************************
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
	typedef string		TAction;
	typedef string		TEntity;
	typedef string		TUpgrade;
	//typedef CostType	TCost;
	typedef int			TPlayer;
	typedef int			TID;

	/**************************************************************** Conditions Definitions****************************************************************/
#define ExactSimilarity 100

	/**************************************************************** Game State Definitions ****************************************************************/

	///> alias=ShallowFeaturesEx(vector(float))
	typedef Serialization::SVector<float>		ShallowFeaturesEx;
	///> alias=DeepFeaturesEx(vector(float))
	typedef Serialization::SVector<float>		DeepFeaturesEx;

	/*********************************************************** Learning From Human Demonstration Definitions ***********************************************************/

	///> alias=GoalSatisfactionEx(vector(int))
	typedef Serialization::SVector<int>				GoalSatisfactionEx;
	///> alias=SequentialPlan(vector(PlanStepEx*))
	typedef	Serialization::SVector<PlanStepEx*>		SequentialPlan;

	///> class=RawPlanEx
	class RawPlanEx : public Serialization::UserObject
	{
		OBJECT_SERIALIZABLE(RawPlanEx);

	public:
		///> type=GoalEx*
		GoalEx*			Goal;
		///> type=SequentialPlan
		SequentialPlan	sPlan;

		RawPlanEx(){}
		RawPlanEx(GoalEx* p_goal, SequentialPlan p_sPlan): Goal(p_goal), sPlan(p_sPlan){}

	protected:
		void InitializeAddressesAux() { AddMemberAddress(2, &Goal, &sPlan); }
	};

	///> class=RawCaseEx
	class RawCaseEx : public Serialization::UserObject
	{
		OBJECT_SERIALIZABLE(RawCaseEx);

	public:
		///> type=RawPlanEx
		RawPlanEx		rawPlan;
		///> type=GameStateEx*
		GameStateEx*	gameState;

		RawCaseEx() {}
		RawCaseEx(RawPlanEx p_rawPlan, GameStateEx*	p_gameState): rawPlan(p_rawPlan), gameState(p_gameState){}
		
	protected:
		void InitializeAddressesAux() { AddMemberAddress(2, &rawPlan, &gameState); }
		//----------------------------------------------------------------------------------------------
	};

	///> class=CookedPlan
	class CookedPlan : public Serialization::UserObject
	{
	public:
		///> type=GoalEx*
		GoalEx*			Goal;
		///> type=GameStateEx*
		GameStateEx*	gameState;
		///> type=PlanGraph*
		PlanGraph*		pPlan;

		CookedPlan(){}
		CookedPlan(GoalEx* p_goal, PlanGraph* p_pPlan, GameStateEx* p_gameState): Goal(p_goal), pPlan(p_pPlan), gameState(p_gameState) {}
		//----------------------------------------------------------------------------------------------
		// Serialization
	public:
		string      TypeName()  { return "CookedPlan"; }
		int         TypeSize()  { return sizeof(CookedPlan); }
		Serialization::UserObject* Prototype() { return new CookedPlan; }	
	protected:
		void InitializeAddressesAux() { AddMemberAddress(3, &Goal, &gameState, &pPlan); }
		//----------------------------------------------------------------------------------------------
	};

	///> class=CookedCase
	struct CookedCase : public Serialization::UserObject
	{
		OBJECT_SERIALIZABLE(CookedCase);

	public:
		///> type=RawCaseEx*
		RawCaseEx*	rawCase;
		///> type=PlanGraph*
		PlanGraph*	dGraph;

		CookedCase(){}
		CookedCase(RawCaseEx* p_rawCase, PlanGraph* p_dGraph): rawCase(p_rawCase), dGraph(p_dGraph) {}
	
	protected:
		void InitializeAddressesAux() { AddMemberAddress(2, &rawCase, &dGraph); }
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
}

#endif	// ENGINEDATA_H*******************************
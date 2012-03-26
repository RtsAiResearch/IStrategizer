///> [Serializable]
#ifndef CASEEX_H
#define CASEEX_H

#include <vector>

class GoalEx;
class PlanGraph;
class GameStateEx;
class CookedPlan;

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

#ifndef PLANGRAPH_H
#include "PlanGraph.h"
#endif

#ifndef GOALEX_H
#include "GoalEx.h"
#endif

#ifndef SPAIR_H
#include "SPair.h"
#endif

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif


using namespace std;
using namespace MetaData;


///> class=CaseEx
class CaseEx : public UserObject
{
private:
    ///> type=PlanGraph*
    PlanGraph*				_planGraph;
    ///> type=GoalEx*
    GoalEx*					_goal;
    ///> type=GameStateEx*
    GameStateEx*			_gameState;
	///> type=int
	int						_trialCount;
	///> type=int
	int						_successCount;
	
public:
    CaseEx() : _planGraph(NULL), _goal(NULL), _gameState(NULL), _trialCount(1), _successCount(1) {};
    CaseEx(PlanGraph* p_planGraph, GoalEx* p_goal, GameStateEx* p_gameState, int p_trialCount, int p_successCount);
    //----------------------------------------------------------------------------------------------------------------------------------------------------
    PlanGraph*      GetPlanGraph() const { return _planGraph; }
    GoalEx*         Goal() const { return _goal; }
    GameStateEx*    GameState() const { return _gameState; }
	int				TrialCount() const { return _trialCount; }
	int				SuccessCount() const { return _successCount; }
	void			TrialCount(int val) { _trialCount = val; }
	void			SuccessCount(int val) { _successCount = val; }

    static CaseEx*  From(CookedPlan* p_cookedPlan);
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
    // Serialization
    string          TypeName()  { return "CaseEx"; }
    UserObject*     Prototype() { return new CaseEx; }
    int             TypeSize()  { return sizeof(CaseEx); }
protected:
    void            InitializeAddressesAux();
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
};
#endif	// CASEEX_H
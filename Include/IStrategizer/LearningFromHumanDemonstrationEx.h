#ifndef LEARNINGFROMHUMANDEMONSTRATIONEX_H
#define LEARNINGFROMHUMANDEMONSTRATIONEX_H

#include <vector>
#include <map>
#include "EngineData.h"
#include "CompositeExpression.h"
#include "CaseLearningHelper.h"

using namespace std;

class CaseBaseEx;
class TripleEx;
class TraceEx;
class Action;

namespace OLCBP
{
    class RetainerEx;
}

const int TIME_STAMP_SIZE_EX = 1;

class LearningFromHumanDemonstrationEx
{
private:
    CaseLearningHelper*	_helper;
    OLCBP::RetainerEx*  _retainer;

    vector<RawCaseEx*>	LearnRawCases(vector<TraceEx*>& p_traces);
    void				AddAction(RawCaseEx* p_case, int p_actionId, PlanStepParameters& p_params, int p_traceId);
    CookedCase*			DependencyGraphGeneration(RawCaseEx* p_rawCases);
    bool				Depends(CompositeExpression* p_candidateNode, CompositeExpression* p_dependentNode, vector<Expression*>& p_matchedConditions);
    void				NecessaryStepsExtraction(PlanGraph* p_graph, int p_sIndex, SequentialPlan& p_fSteps, const SequentialPlan& p_steps);
    void				UnnecessaryStepsElimination(CookedCase* p_case);
    CookedPlan*			PlanParallelization(PlanGraph* p_graph, RawPlanEx* p_steps);
    void				HierarchicalComposition(CookedPlan* p_plan, const vector<CookedPlan*>& p_plans, int p_index);

public:
    		    LearningFromHumanDemonstrationEx(PlayerType p_player, PlayerType p_enemy);
    CaseBaseEx*	CaseBaseAcquisition();
    void        LearnCookedCases( vector<CookedPlan*>& p_cookedPlans );
    virtual     ~LearningFromHumanDemonstrationEx();
};
#endif	// LEARNINGFROMHUMANDEMONSTRATIONEX_H
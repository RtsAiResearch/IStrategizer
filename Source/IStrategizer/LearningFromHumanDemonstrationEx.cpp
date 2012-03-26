#include "LearningFromHumanDemonstrationEx.h"
#include "TraceEx.h"
#include "CaseBaseEx.h"
#include "ActionFactory.h"
#include "CaseLearningHelper.h"
#include "Diagraph.h"
#include "Action.h"
#include "GoalFactory.h"
#include "GoalEx.h"
#include "PlanGraph.h"
#include "ConditionEx.h"

#ifndef RETAINEREX_H
#include "RetainerEx.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif

#include <queue>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;
using namespace OLCBP;

LearningFromHumanDemonstrationEx::LearningFromHumanDemonstrationEx(PlayerType p_player, PlayerType p_enemy)
{
    _helper     = new CaseLearningHelper(p_player, p_enemy);
    _retainer   = new RetainerEx("CaseBase0.cb");
	_retainer->ReadCaseBase();
}
//------------------------------------------------------------------------------------------------
CaseBaseEx* LearningFromHumanDemonstrationEx::CaseBaseAcquisition()
{
 //   vector<CookedCase*>		m_cookedCases;
 //   vector<CookedPlan*>		m_cookedPlans;
	//vector<PlanStepEx*>		m_steps;

 //   vector<RawCaseEx*> m_rawCases = LearnRawCases(_helper->ObservedTraces());

 //   m_cookedCases.resize(m_rawCases.size());

 //   for (int i = 0; i < m_rawCases.size(); ++i)
 //   {
 //       m_cookedCases[i] = DependencyGraphGeneration(m_rawCases[i]);
 //   }

 //   for (int i = 0; i < m_cookedCases.size(); ++i)
 //   {
 //       UnnecessaryStepsElimination(m_cookedCases[i]);
 //   }

 //   for (int i = 0; i < m_cookedCases.size(); ++i)
 //   {
 //       m_cookedCases[i] = DependencyGraphGeneration(m_cookedCases[i]->rawCase);
 //   }

 //   for (int i = 0; i < m_cookedCases.size(); ++i)
 //   {
	//	m_cookedPlans.push_back( new CookedPlan(m_cookedCases[i]->rawCase->rawPlan.Goal, m_cookedCases[i]->dGraph, m_cookedCases[i]->rawCase->gameState));
 //   }

	//// Initial code of adding plans to High Level Steps using Medium Level
	//// FIXME: needs to be recursive.
	//for (int i = 0; i < m_cookedPlans.size(); ++i)
	//{
	//	if (m_cookedPlans[i]->Goal->LevelType() == SLEVELTYPE_High)
	//	{
	//		vector<Expression*> m_usedConditions;
	//		for (int j = m_cookedPlans.size() - 1; j > 0; --j)
	//		{
	//			if(Depends(m_cookedPlans[j]->Goal->PostConditions(), m_cookedPlans[i]->Goal->SuccessConditions(), m_usedConditions))
	//			{
	//				m_cookedPlans[i]->pPlan = new PlanGraph(m_cookedPlans[j]->Goal);
	//				break;
	//			}
	//		}
	//	}
	//}

	//for (int i = 0; i < m_cookedPlans.size(); ++i)
	//{
	//	HierarchicalComposition(m_cookedPlans[i], m_cookedPlans, i);
	//}

 //   LearnCookedCases(m_cookedPlans);
	//
    return NULL;
}
//------------------------------------------------------------------------------------------------
vector<RawCaseEx*> LearningFromHumanDemonstrationEx::LearnRawCases(vector<TraceEx*>& p_traces)
{
    unsigned            m_rowSize = _helper->GoalSatisfactionRow().GetRowSize();
    vector<RawCaseEx*>	m_learntRawCases = vector<RawCaseEx*>();
    vector<RawCaseEx*>	m_currentCases = vector<RawCaseEx*>(m_rowSize, NULL);
    RawPlanEx			m_tempRPlan;
    unsigned			i, g;

    for (i = 0; i < p_traces.size(); ++i)
    {
        for (g = 0; g < m_rowSize; ++g)
        {
            if (!p_traces[i]->GoalSatisfaction()[g] && !m_currentCases[g])
            {
                m_tempRPlan = RawPlanEx(NULL, SequentialPlan());
                m_currentCases[g] = new RawCaseEx(m_tempRPlan, p_traces[i]->GameState());
                //m_currentCases[g] = new RawCaseEx(m_tempRPlan, NULL);
                AddAction(m_currentCases[g], p_traces[i]->AbstractActionId(), p_traces[i]->AbstractParametersId(), i);
            }
            else if(!p_traces[i]->GoalSatisfaction()[g] && m_currentCases[g])
            {
                AddAction(m_currentCases[g], p_traces[i]->AbstractActionId(), p_traces[i]->AbstractParametersId(), i);
            }
            else if (p_traces[i]->GoalSatisfaction()[g] && m_currentCases[g])
            {
                m_currentCases[g]->rawPlan.Goal = _helper->GoalSatisfactionRow().GetGoal(g);
                AddAction(m_currentCases[g], p_traces[i]->AbstractActionId(), p_traces[i]->AbstractParametersId(), i);
                m_learntRawCases.push_back(m_currentCases[g]);
                m_currentCases[g] = NULL;
            }
        }
    }

    return m_learntRawCases;
}
//------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstrationEx::AddAction(RawCaseEx* p_case, int p_actionId, PlanStepParameters& p_params, int p_traceId)
{
	Action* action = g_ActionFactory.GetAction((ActionType)p_actionId, p_params);
	action->Data(p_traceId);
	action->InitializeConditions();

    p_case->rawPlan.sPlan.push_back(action);
}
//------------------------------------------------------------------------------------------------
CookedCase* LearningFromHumanDemonstrationEx::DependencyGraphGeneration(RawCaseEx* p_rawCase)
{
	SequentialPlan			m_planSteps = vector<PlanStepEx*>(p_rawCase->rawPlan.sPlan.size());
    unsigned				m_stepsCount = m_planSteps.size();
    PlanGraph*				m_graph;
    Action					*aJ;
    unsigned				i, j;
	vector<Expression*>		m_matchedConditions;

	for ( i = 0; i < p_rawCase->rawPlan.sPlan.size(); ++i)
	{
		m_planSteps[i] = static_cast<PlanStepEx*>(p_rawCase->rawPlan.sPlan[i]->Clone());
	}

	m_graph = new PlanGraph(m_planSteps);

    for (i = 0; i < m_stepsCount; ++i)
    {
        for (j = 0; j < m_stepsCount; ++j)
        {
            if (i != j)
            {
                aJ = (Action*)m_planSteps[j];

                /*if (Depends(m_planSteps[i]->PostConditions(), aJ->Preconditions(), m_matchedConditions))
                {
                    m_graph->Connect(i, j, m_matchedConditions);
					m_matchedConditions.clear();
				}*/
            }
        }
    }

    return new CookedCase(p_rawCase, m_graph);
}
//--------------------------------------------------------------------------------------------------------------
bool LearningFromHumanDemonstrationEx::Depends(CompositeExpression* p_candidateNode, CompositeExpression* p_dependentNode, vector<Expression*>& p_matchedConditions)
{
	if (p_candidateNode && p_dependentNode)
	{
		vector< pair<Expression*,Expression*> > m_candidateConditions;
		ConditionEx* m_precondition;
		ConditionEx* m_postCondition;

		p_candidateNode->PartiallyEquals(p_dependentNode, m_candidateConditions);
		for (unsigned i = 0; i < m_candidateConditions.size(); ++i)
		{
			m_precondition = (ConditionEx*)m_candidateConditions[i].second;
			m_postCondition = (ConditionEx*)m_candidateConditions[i].first;

			if (m_postCondition->Consume( (m_precondition->ContainsParameter(PARAM_Amount)) ? m_precondition->Parameter(PARAM_Amount) : NULL ))
			{
				p_matchedConditions.push_back(m_candidateConditions[i].first);
				p_dependentNode->RemoveExpression(m_candidateConditions[i].second);
			}
		}

		return (!p_matchedConditions.empty());
	}
	else
	{
		return false;
	}
}
//------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstrationEx::UnnecessaryStepsElimination(CookedCase* p_case)
{
 //   SequentialPlan							steps = vector<PlanStepEx*>(p_case->rawCase->rawPlan.sPlan.size());
 //   SequentialPlan							fSteps;
 //   vector<int>								rSteps;
 //   int										i, initSize;
	//vector<Expression*> m_usedConditions;

	//for ( i = 0; i < p_case->rawCase->rawPlan.sPlan.size(); ++i)
	//{
	//	steps[i] = static_cast<PlanStepEx*>(p_case->rawCase->rawPlan.sPlan[i]->Clone());
	//}

 //   // Extract direct steps
 //   for (i = 0; i < steps.size(); ++i)
 //   {
 //       if (Depends(steps[i]->PostConditions(), p_case->rawCase->rawPlan.Goal->PostConditions(), m_usedConditions))
 //       {
 //           fSteps.push_back(steps[i]);
 //           rSteps.push_back(i);
	//		m_usedConditions.clear();
 //       }
 //   }

 //   initSize = rSteps.size();
	//steps.clear();
	//
	//for ( i = 0; i < p_case->rawCase->rawPlan.sPlan.size(); ++i)
	//{
	//	steps.push_back(static_cast<PlanStepEx*>(p_case->rawCase->rawPlan.sPlan[i]->Clone()));
	//}

 //   // Extract indirect steps
 //   for (i = 0; i < initSize; ++i)
 //   {
 //       NecessaryStepsExtraction(p_case->dGraph, rSteps[i], fSteps, steps);
 //   }

	//p_case->rawCase->rawPlan.sPlan.clear();
	//p_case->rawCase->rawPlan.sPlan.insert(p_case->rawCase->rawPlan.sPlan.begin(), fSteps.begin(), fSteps.end());

 //   //if(!fSteps.empty())
 //   //{
 //   //    // use the game state of the first action, the index of the trace is stored in the data storage
 //   //    TraceEx* firstTrace = _helper->ObservedTraces()[fSteps[0]->Data()];
 //   //    p_case->rawCase->gameState = firstTrace->GameState();
 //   //}

	//p_case->dGraph = new PlanGraph(fSteps);
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstrationEx::NecessaryStepsExtraction(PlanGraph* p_graph, int p_sIndex, SequentialPlan& p_fSteps, const SequentialPlan& p_steps)
{
    vector< pair<int, PlanStepEx*> >	rSteps;
    unsigned							i;
	vector<int>							toDelete;

    for (i = 0; i < p_graph->Size(); ++i)
    {
        if (i != p_sIndex && p_graph->IsConnected(i, p_sIndex))
        {
            if (find(p_fSteps.begin(), p_fSteps.end(), p_steps[i]) == p_fSteps.end())
            {
                rSteps.push_back(make_pair(i, p_steps[i]));
                p_fSteps.insert(p_fSteps.begin(), p_steps[i]);
            }
        }
    }

    for (i = 0; i < rSteps.size(); ++i)
    {
        NecessaryStepsExtraction(p_graph, rSteps[i].first, p_fSteps, p_steps);
    }
}
//--------------------------------------------------------------------------------------------------------------
void LearningFromHumanDemonstrationEx::HierarchicalComposition(CookedPlan* p_plan, const vector<CookedPlan*>& p_plans, int p_index)
{
	int				m_maxSubgraphIndex = INT_MAX;
	int				m_maxMatchedCount = 0;
	int				m_currentMatchedCount;
	vector<int>		m_cMatchedIndexes;
	vector<int>		m_mMatchedIndexes;
	
	for (unsigned i = 0; i < p_plans.size(); ++i)
	{
		if (i != p_index && p_plan->pPlan->IsSubGraph((*p_plans[i]->pPlan), m_cMatchedIndexes, m_currentMatchedCount))
		{
			if (m_currentMatchedCount > m_maxMatchedCount)
			{
				m_maxSubgraphIndex = i;
				m_mMatchedIndexes = m_cMatchedIndexes;
				m_maxMatchedCount = m_currentMatchedCount;
			}
		}
	}

	if (m_maxSubgraphIndex != INT_MAX)
	{
		p_plan->pPlan->SubGraphSubstitution(m_mMatchedIndexes, p_plans[m_maxSubgraphIndex]->Goal);
	}
}
//----------------------------------------------------------------------------------------------
void LearningFromHumanDemonstrationEx::LearnCookedCases( vector<CookedPlan*>& p_cookedPlans )
{
    CaseEx*     m_learntCase    = NULL;
    for(int i = 0, size = p_cookedPlans.size(); i < size; ++i)
    {
        m_learntCase = CaseEx::From(p_cookedPlans[i]);
        _retainer->Retain(m_learntCase);
    }

    _retainer->Flush();
}
//--------------------------------------------------------------------------------------------------------------
LearningFromHumanDemonstrationEx::~LearningFromHumanDemonstrationEx()
{
    Toolbox::MemoryClean(_helper);
    Toolbox::MemoryClean(_retainer);
}
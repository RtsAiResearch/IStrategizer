#ifndef CASEVIEW_H
#define CASEVIEW_H

#include <QWidget>
#include "ui_CaseView.h"

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

#include <string>
#include <hash_map>
using namespace stdext;
using namespace DataStructure;

class CaseEx;
class GoalEx; 
class GameStateEx;

#define IdLookupFile "IdLookup.txt"

class GraphScene;
class GoalEx;
class PlanGraphView;

class CaseView : public QWidget
{
    Q_OBJECT

public:
    CaseView(CrossMap<unsigned, string>* p_idLookup, QWidget *parent = 0);
    ~CaseView();
    void View(CaseEx* p_case);
	void SetMode(int p_mode);

private:
	CrossMap<unsigned, string>* m_idLookup;
	GraphScene*					m_graphScene;
	PlanGraphView*				m_graphView;
    Ui::CaseViewClass			ui;
	CaseEx*						m_currentCase;

    void ViewGoal(GoalEx* p_goal);
    void ViewGoalParameters(PlanStepParameters* p_params);
    void ViewGameState(GameStateEx* p_gameState);
	void ViewPlanGraph(GoalEx* p_caseGoal, PlanGraph* p_planGraph);
    void InitializeIdLookup();
	void CreatePlanView();
private slots:
	void OnCellChanged(int p_row, int p_column);
};

#endif // CASEVIEW_H

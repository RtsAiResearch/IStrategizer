#ifndef PLANGRAPHVIEW_H
#define PLANGRAPHVIEW_H

#include <QWidget>
#include <hash_map>
#include <string>
using namespace std;
using namespace stdext;
#include "ui_PlanGraphView.h"
#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

class PlanGraph;
class GraphScene;
class QGraphicsView;
class PlanStepView;
class GraphNodeView;
class GoalEx;

using namespace DataStructure;

class PlanGraphView : public QWidget
{
    Q_OBJECT

public:
    PlanGraphView(GraphScene* p_scene, CrossMap<unsigned, string>* p_idLookup, QWidget *p_parent = 0);
    ~PlanGraphView();

	void View(GoalEx* p_caseGoal, PlanGraph* p_planGraph);
	void SetMode(int p_mode);

private:
    Ui::PlanGraphViewClass	ui;
	QGraphicsView*			m_graphicsView;		
	GraphScene*				m_scene;
	PlanStepView*			m_planStepView;

private slots:
	void HandleNodeSelected(GraphNodeView* p_node);
};

#endif // PLANGRAPHVIEW_H

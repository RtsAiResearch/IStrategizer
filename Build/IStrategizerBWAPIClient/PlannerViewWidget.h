#ifndef PLANNERVIEWWIDGET_H
#define PLANNERVIEWWIDGET_H

#include <QWidget>
#include "ui_PlannerViewWidget.h"

namespace IStrategizer
{
	class OnlinePlanExpansionExecutionEx;
	class PlanStepEx;
};

class PlannerViewWidget : public QWidget
{
	Q_OBJECT

public:
	PlannerViewWidget(QWidget *p_pParent = 0);
	~PlannerViewWidget();
	void update();
	void DrawPlanTree();
	QTreeWidgetItem * TreeItemFromPlanStep(const IStrategizer::PlanStepEx* p_pPlanStep);
	void Planner(const IStrategizer::OnlinePlanExpansionExecutionEx *p_pPlanner) { m_pPlanner = p_pPlanner; }
private:
	Ui::PlannerViewWidget ui;
	const IStrategizer::OnlinePlanExpansionExecutionEx *m_pPlanner;
};

#endif // PLANNERVIEWWIDGET_H

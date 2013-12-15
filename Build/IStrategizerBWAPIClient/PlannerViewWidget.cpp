#include "PlannerViewWidget.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "OnlinePlanExpansionExecutionEx.h"
#include "PlanStepEx.h"
#include <QString>
#include <cassert>

using namespace IStrategizer;

PlannerViewWidget::PlannerViewWidget(QWidget *p_pParent)
	: QWidget(p_pParent)
{
	ui.setupUi(this);
}

PlannerViewWidget::~PlannerViewWidget()
{

}
//////////////////////////////////////////////////////////////////////////
void PlannerViewWidget::update()
{
	if (nullptr != m_pPlanner)
		DrawPlanTree();
}
//////////////////////////////////////////////////////////////////////////
void PlannerViewWidget::DrawPlanTree()
{
	QTreeWidget *pTreeWdgt = ui.graphWidget;
	QTreeWidgetItem *pTreeItem = nullptr;
	const PlanStepEx* pPlanStep = nullptr;

	assert(pTreeWdgt);
	pTreeWdgt->clear();

	assert(m_pPlanner);
	if (m_pPlanner->PlanRoot() != nullptr)
	{
		pPlanStep = m_pPlanner->PlanRoot()->PlanStep();
		assert(pPlanStep);
		pTreeItem = TreeItemFromPlanStep(pPlanStep);
		assert(pTreeItem);

		pTreeWdgt->insertTopLevelItem(0, pTreeItem);
	}
}
//////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* PlannerViewWidget::TreeItemFromPlanStep(const PlanStepEx* p_pPlanStep)
{
	QTreeWidgetItem *pTreeItem = new QTreeWidgetItem();
	pTreeItem->setText(0, QString::fromStdString(p_pPlanStep->ToString()));

	return pTreeItem;
}

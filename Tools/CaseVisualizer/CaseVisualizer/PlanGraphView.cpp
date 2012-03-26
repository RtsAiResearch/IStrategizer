#ifndef PLANGRAPHVIEW_H
#include "PlanGraphView.h"
#endif
#ifndef PLANGRAPH_H
#include "PlanGraph.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef GRAPHSCENE_H
#include "GraphScene.h"
#endif
#ifndef PLANSTEPVIEW_H
#include "PlanStepView.h"
#endif
#ifndef GRAPHNODEVIEW_H
#include "GraphNodeView.h"
#endif
#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif
using namespace DataStructure;

#include <QGridLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QGraphicsRectItem>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QToolButton>

PlanGraphView::PlanGraphView(GraphScene* p_scene, CrossMap<unsigned, string>* p_idLookup, QWidget *p_parent)
: QWidget(p_parent)
{
	ui.setupUi(this);
	
	QHBoxLayout* layout = new QHBoxLayout;
	m_planStepView = new PlanStepView(p_idLookup);
	m_graphicsView = new QGraphicsView();
	m_graphicsView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	m_scene = p_scene;
	m_scene->setSceneRect(0, 0, 1000, 1000);

	m_graphicsView->setScene(m_scene);
	//m_graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	m_graphicsView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
	
	m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	layout->setMargin(0);
	layout->addWidget(m_graphicsView);
	layout->addWidget(m_planStepView);
	
	QRectF r1 = m_planStepView->geometry();
	QRectF r2 = m_graphicsView->geometry();

	setLayout(layout);

	connect(m_scene, SIGNAL(NodeSelected(GraphNodeView*)), SLOT(HandleNodeSelected(GraphNodeView*)));
}
//----------------------------------------------------------------------------------------------
void PlanGraphView::SetMode(int p_mode) 
{
	m_scene->Mode((GraphScene::PointerMode)p_mode); 
}
//----------------------------------------------------------------------------------------------
void PlanGraphView::View(GoalEx* p_caseGoal, PlanGraph* p_planGraph)
{
	m_scene->View(p_caseGoal, p_planGraph);
}
//----------------------------------------------------------------------------------------------
void PlanGraphView::HandleNodeSelected(GraphNodeView* p_node)
{
	if(p_node == NULL)
		m_planStepView->View(NULL);
	else
		m_planStepView->View(p_node->NodeModel());
}	
//----------------------------------------------------------------------------------------------
PlanGraphView::~PlanGraphView()
{
}

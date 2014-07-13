#include "stdafx.h"
#ifndef PLANGRAPHVIEW_H
#include "PlanGraphView.h"
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

#include "GraphGraphicsView.h"

using namespace IStrategizer;
using namespace std;

PlanGraphView::PlanGraphView(GraphScene* p_scene, CrossMap<unsigned, string>* p_idLookup, QWidget *p_parent)
: QWidget(p_parent)
{
    m_ui.setupUi(this);
    
    m_pPlanStepView = new PlanStepView(p_idLookup);

    m_pScene = p_scene;
    m_pScene->setSceneRect(0, 0, 1000, 1000);
    
    m_pGraphicsView = new GraphGraphicsView();
    m_pGraphicsView->setScene(m_pScene);
    m_pGraphicsView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);

    
    ((QHBoxLayout*)m_ui.planGraphViewInnerLayout)->addWidget(m_pGraphicsView);
    ((QHBoxLayout*)m_ui.planGraphViewInnerLayout)->addWidget(m_pPlanStepView);

    connect(m_pScene, SIGNAL(NodeSelected(GraphNodeView*)), SLOT(HandleNodeSelected(GraphNodeView*)));

    m_updateTimerId = startTimer(GraphUpdateIntervalMs);
}
//----------------------------------------------------------------------------------------------
void PlanGraphView::SetMode(GraphScene::PointerMode mode) 
{
    m_pScene->Mode((GraphScene::PointerMode)mode); 
}
//----------------------------------------------------------------------------------------------
void PlanGraphView::View(IOlcbpPlan* pPlanGraph, ConstOlcbpPlanContextPtr pPlanContext)
{
    m_pScene->View(pPlanGraph, pPlanContext);
}
//----------------------------------------------------------------------------------------------
void PlanGraphView::HandleNodeSelected(GraphNodeView* p_node)
{
    if(p_node == NULL)
        m_pPlanStepView->View(NULL);
    else
        m_pPlanStepView->View(p_node->NodeModel());
}
//----------------------------------------------------------------------------------------------
void PlanGraphView::NotifyGraphStructureChange(IOlcbpPlan* pPlan)
{
    m_pScene->NotifyGraphStructureChange(pPlan);
}
//----------------------------------------------------------------------------------------------
void PlanGraphView::timerEvent(QTimerEvent *pEvt)
{
    if (pEvt->timerId() == m_updateTimerId)
        m_pScene->UpdateGraph();
}

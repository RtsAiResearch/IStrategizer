#include "stdafx.h"
#ifndef GRAPHSCENE_H
#include "GraphScene.h"
#endif
#ifndef PLANSTEPEX_H
#include "PlanStepEx.h"
#endif
#ifndef GRAPHNODEVIEW_H
#include "GraphNodeView.h"
#endif
#ifndef GRAPHEDGEVIEW_H
#include "GraphEdgeView.h"
#endif
#ifndef GOALEX_H
#include "GoalEx.h"
#endif
#ifndef ACTION_H
#include "Action.h"
#endif
#ifndef CHOOSEPLANSTEPDIALOG_H
#include "ChoosePlanStepDialog.h"
#endif
#ifndef GOALFACTORY_H
#include "GoalFactory.h"
#endif
#ifndef ACTIONFACTORY_H
#include "ActionFactory.h"
#endif
#include "IStrategizerException.h"
#include "Logger.h"
#include <list>

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

GraphScene::GraphScene(CrossMap<unsigned, string>* pIdLookup, QObject *pParent) : QGraphicsScene(pParent)
{
    m_horizontalNodeSpacing = DefaultHorizontalNodeSpacing;
    m_verticalNodeSpacing = DefaultVerticalNodeSpacing;
    m_pNodeMenu = nullptr;
    m_pChoosePlanStepDlg = new ChoosePlanStepDialog(pIdLookup, true, true);
    m_pointerMode = PTRMODE_Move;
    m_pConnectionLine = nullptr;
    m_pGraph = nullptr;
    m_cellSize = DefaultGridCellSize;
    m_lastCtxtMenuScreenPos = QPointF(0.0, 0.0);

    connect(this, SIGNAL(selectionChanged()), SLOT(NodeSelected()));

    CreateNodeMenu();
    CreateEdgeMenu();
    CreateSceneMenu();
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateNodeMenu()
{
    QAction *pDuplicateAction = new QAction(tr("Duplicate"), this);
    pDuplicateAction->setStatusTip(tr("Duplicates graph node"));
    connect(pDuplicateAction, SIGNAL(triggered()), this, SLOT(DuplicateNode()));

    QAction *pDeleteAction = new QAction(tr("Delete"), this);
    pDuplicateAction->setStatusTip(tr("Deletes graph node"));
    connect(pDeleteAction, SIGNAL(triggered()), this, SLOT(DeleteNode()));

    QAction *pDisconnectAction = new QAction(tr("Disconnect"), this);
    pDisconnectAction->setStatusTip(tr("Disconnects the node from the graph"));
    connect(pDisconnectAction, SIGNAL(triggered()), this, SLOT(DisconnectNode()));

    m_pNodeMenu = new QMenu(tr("Node Menu"));
    m_pNodeMenu->setTearOffEnabled(true);
    m_pNodeMenu->addAction(pDeleteAction);
    m_pNodeMenu->addAction(pDuplicateAction);
    m_pNodeMenu->addAction(pDisconnectAction);
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateEdgeMenu()
{
    QAction *pDeleteAction = new QAction(tr("&Delete"), this);
    pDeleteAction->setStatusTip(tr("Deletes graph edge"));
    pDeleteAction->setShortcut(QString::fromLocal8Bit("D"));
    connect(pDeleteAction, SIGNAL(triggered()), this, SLOT(DeleteEdge()));

    m_pEdgeMenu = new QMenu(tr("Edge Menu"));
    m_pEdgeMenu->setTearOffEnabled(true);
    m_pEdgeMenu->addAction(pDeleteAction);
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateSceneMenu()
{
    QAction *pNewNodeAction = new QAction(tr("N&ew Node"), this);
    pNewNodeAction->setStatusTip(tr("Creates new plan graph node"));
    connect(pNewNodeAction, SIGNAL(triggered()), this, SLOT(NewNode()));

    QAction *pLayoutGraph = new QAction(tr("L&ayout Plan"), this);
    pLayoutGraph->setStatusTip(tr("Layout plan graph in the scene"));
    connect(pLayoutGraph, SIGNAL(triggered()), this, SLOT(OnGraphStructureChange()));

    m_pSceneMenu = new QMenu(tr("Scene Menu"));
    m_pSceneMenu->setTearOffEnabled(true);
    m_pSceneMenu->addAction(pNewNodeAction);
    m_pSceneMenu->addAction(pLayoutGraph);
}
//----------------------------------------------------------------------------------------------
void GraphScene::View(IOlcbpPlan* pPlan, ConstOlcbpPlanContextPtr pPlanContext)
{
    m_pGraph = pPlan;
    m_pPlanContext = pPlanContext;
    QApplication::postEvent(this, new QGraphStructureChangeEvent(pPlan));
}
//----------------------------------------------------------------------------------------------
void GraphScene::OnGraphStructureChange()
{
    clear();

    if (m_pGraph)
    {
        m_pGraph->Lock();

        ConstructGraph();
        LayoutGraph();

        m_pGraph->Unlock();
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::ConstructGraph()
{
    if (m_pGraph == nullptr && !m_nodeIdToNodeViewMap.empty())
        m_nodeIdToNodeViewMap.clear();

    if(m_pGraph == nullptr)
        return;

    ComputeGraphLevels();

    m_nodeIdToNodeViewMap.clear();
    for (unsigned level = 0 ; level < m_graphLevels.size(); ++level)
    {
        for (NodeID nodeId : m_graphLevels[level])
        {
            GraphNodeView* pNodeView = new GraphNodeView(m_pGraph->GetNode(nodeId),
                (m_pPlanContext != nullptr ? &m_pPlanContext->Data.at(nodeId) : nullptr)
                , m_pNodeMenu, nullptr);
            m_nodeIdToNodeViewMap[nodeId] = pNodeView;
            addItem(pNodeView);
        }
    }

    ConnectGraphNodes();
}
//----------------------------------------------------------------------------------------------
void IStrategizer::GraphScene::ComputeGraphLevels()
{
    IOlcbpPlan::NodeSerializedSet roots = m_pGraph->GetOrphanNodes();
    IOlcbpPlan::NodeSerializedSet visitedNodes;

    typedef unsigned NodeLevel;
    deque< pair<NodeID, NodeLevel> > Q;

    for (NodeID nodeId : roots)
    {
        Q.push_back(make_pair(nodeId, 0));
        visitedNodes.insert(nodeId);
    }

    for(unsigned i = 0; i < m_graphLevels.size(); ++i)
        m_graphLevels[i].clear();
    m_graphLevels.clear();

    list<NodeID> orderedCaseChildren;
    list<NodeID> orderedSnippetChildren;

    while(!Q.empty())
    {
        NodeID currNodeId = Q.front().first;
        NodeLevel nodeLevel = Q.front().second;

        Q.pop_front();

        if(nodeLevel >= m_graphLevels.size())
            m_graphLevels.resize(nodeLevel + 1);

        m_graphLevels[nodeLevel].push_back(currNodeId);

        IOlcbpPlan::NodeSerializedSet children = m_pGraph->GetAdjacentNodes(currNodeId);

        if (m_pPlanContext != nullptr)
        {
            orderedCaseChildren.clear();
            orderedSnippetChildren.clear();

            for (NodeID nodeId : children)
            {
                if (visitedNodes.count(nodeId) == 0)
                {
                    visitedNodes.insert(nodeId);

                    if (m_pPlanContext->Data.at(nodeId).SatisfyingGoal != currNodeId)
                    {
                        if (IsGoalNode(nodeId))
                            orderedCaseChildren.push_front(nodeId);
                        else
                            orderedCaseChildren.push_back(nodeId);
                    }
                    else
                    {
                        if (IsGoalNode(nodeId))
                            orderedSnippetChildren.push_front(nodeId);
                        else
                            orderedSnippetChildren.push_back(nodeId);
                    }
                }
            }

            for (auto nodeId : orderedCaseChildren)
                Q.push_back(make_pair(nodeId, nodeLevel + 1));

            for (auto nodeId : orderedSnippetChildren)
                Q.push_back(make_pair(nodeId, nodeLevel + 1));
        }
        else
        {
            for (NodeID nodeId : children)
            {
                if (visitedNodes.count(nodeId) == 0)
                {
                    visitedNodes.insert(nodeId);
                    Q.push_back(make_pair(nodeId, nodeLevel + 1));
                }
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::ConnectGraphNodes()
{
    if(m_pGraph == nullptr)
        return;

    IOlcbpPlan::NodeSerializedSet nodes = m_pGraph->GetNodes();

    _ASSERTE(nodes.size() == m_pGraph->Size());

    for (NodeID srcNodeId : nodes)
    {
        _ASSERTE(nodes.size() == m_pGraph->Size());
        IOlcbpPlan::NodeSerializedSet adjNodes = m_pGraph->GetAdjacentNodes(srcNodeId);

        for (NodeID destNodeId : adjNodes)
        {
            if (m_nodeIdToNodeViewMap.count(srcNodeId) == 0 ||
                m_nodeIdToNodeViewMap.count(destNodeId) == 0)
            {
                LogInfo("Invalid plan links:\n%s", m_pGraph->ToString().c_str());
                _ASSERTE(!"Invalid plan links detected");
            }

            GraphNodeView* pStart = m_nodeIdToNodeViewMap[srcNodeId];
            GraphNodeView* pEnd = m_nodeIdToNodeViewMap[destNodeId];

            ConnectNodeViews(pStart, pEnd);
        }
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::LayoutGraphInHierarchy()
{
    //if(m_pGraph == nullptr)
    //    return;

    //GmlHelper gml;
    //gml.Load(m_pGraph);
    //gml.Layout();
};
//----------------------------------------------------------------------------------------------
void GraphScene::LayoutGraph()
{
    if(m_pGraph == nullptr)
        return;

    int maximumLevelWidth = 0, runningVerticalPosition = m_verticalNodeSpacing;

    for (unsigned currentLevel = 0 ; currentLevel < m_graphLevels.size(); ++currentLevel)
    {
        maximumLevelWidth = max(maximumLevelWidth, ComputeLevelWidth(currentLevel));
    }

    for (unsigned currentLevel = 0 ; currentLevel < m_graphLevels.size(); ++currentLevel)
    {
        int runningHorizontalPosition = m_horizontalNodeSpacing + (maximumLevelWidth - ComputeLevelWidth(currentLevel)) / 2;

        for (NodeID nodeId : m_graphLevels[currentLevel])
        {
            m_nodeIdToNodeViewMap[nodeId]->setPos(QPointF(runningHorizontalPosition, runningVerticalPosition));
            runningHorizontalPosition += m_nodeIdToNodeViewMap[nodeId]->NodeWidth() + m_horizontalNodeSpacing;
        }

        runningVerticalPosition += ComputeLevelHeight(currentLevel) + m_verticalNodeSpacing;
    }

    this->setSceneRect(0, 0, maximumLevelWidth + m_horizontalNodeSpacing * 2, runningVerticalPosition);
}
//----------------------------------------------------------------------------------------------
int IStrategizer::GraphScene::ComputeLevelWidth(int levelIdx)
{
    int levelWidth = 0;

    // Sum the width of all level nodes
    for (NodeID nodeID : m_graphLevels[levelIdx])
    {
        levelWidth += m_nodeIdToNodeViewMap[nodeID]->NodeWidth();
    }

    // Take into account the inter node horizontal spacing
    levelWidth += ((m_graphLevels[levelIdx].size() - 1) * m_horizontalNodeSpacing);

    return levelWidth;
}
//----------------------------------------------------------------------------------------------
int IStrategizer::GraphScene::ComputeLevelHeight(int levelIdx)
{
    int levelHeight = 0;

    // Level height is the height of the node with the largest height
    for (NodeID nodeID : m_graphLevels[levelIdx])
    {
        levelHeight = max(levelHeight, m_nodeIdToNodeViewMap[nodeID]->NodeHeight());
    }

    return levelHeight;
}
//----------------------------------------------------------------------------------------------
void GraphScene::NotifyGraphStructureChange(IOlcbpPlan* pGraph)
{
    QApplication::postEvent(this, new QGraphStructureChangeEvent(pGraph));
}
//----------------------------------------------------------------------------------------------
void GraphScene::UpdateGraph()
{
    QApplication::postEvent(this, new QEvent((QEvent::Type)SCENEEVT_GraphRedraw));
}
//----------------------------------------------------------------------------------------------
void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *p_mouseEvent)
{
    if (p_mouseEvent->button() != Qt::LeftButton)
        return;

    if(m_pointerMode == MODE_Connect)
    {
        m_pConnectionLine = new QGraphicsLineItem(QLineF(p_mouseEvent->scenePos(), p_mouseEvent->scenePos()));
        m_pConnectionLine->setPen(QPen(QColor("black"), 2));
        addItem(m_pConnectionLine);
    }

    QGraphicsScene::mousePressEvent(p_mouseEvent);
}
//----------------------------------------------------------------------------------------------
void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *p_mouseEvent)
{
    if (m_pointerMode == MODE_Connect && m_pConnectionLine != 0) 
    {
        QLineF newLine(m_pConnectionLine->line().p1(), p_mouseEvent->scenePos());
        m_pConnectionLine->setLine(newLine);
    } 
    else if (m_pointerMode == PTRMODE_Move) 
    {
        QGraphicsScene::mouseMoveEvent(p_mouseEvent);
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *p_mouseEvent)
{
    if (m_pConnectionLine != 0 && m_pointerMode == MODE_Connect) 
    {
        QList<QGraphicsItem *> startItems = items(m_pConnectionLine->line().p1());
        if (startItems.count() && startItems.first() == m_pConnectionLine)
            startItems.removeFirst();

        QList<QGraphicsItem *> endItems = items(m_pConnectionLine->line().p2());
        if (endItems.count() && endItems.first() == m_pConnectionLine)
            endItems.removeFirst();

        removeItem(m_pConnectionLine);
        delete m_pConnectionLine;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first() != endItems.first()) 
        {
            GraphNodeView *pStartNode = qgraphicsitem_cast<GraphNodeView *>(startItems.first());
            GraphNodeView *pEndNode = qgraphicsitem_cast<GraphNodeView *>(endItems.first());

            if (pStartNode != nullptr &&
                pEndNode != nullptr)
            {
                m_pGraph->AddEdge(pStartNode->ModelId(), pEndNode->ModelId());
                ConnectNodeViews(pStartNode, pEndNode);
            }
        }
    }

    m_pConnectionLine = nullptr;
    QGraphicsScene::mouseReleaseEvent(p_mouseEvent);
}
//----------------------------------------------------------------------------------------------
void GraphScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *pEvt)
{
    pEvt->setAccepted(false);

    QGraphicsScene::contextMenuEvent(pEvt);

    if(!pEvt->isAccepted())
    {
        pEvt->setAccepted(true);
        if(m_pSceneMenu != NULL)
        {
            m_lastCtxtMenuScreenPos = pEvt->scenePos();
            m_pSceneMenu->exec(pEvt->screenPos());
        }
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::NewNode()
{
    if(m_pChoosePlanStepDlg->exec() == QDialog::Accepted)
    {
        unsigned planStepId = m_pChoosePlanStepDlg->SelectedPlanStepId();
        NodeValue pNodeModel = NULL;

        if(BELONG(GoalType, planStepId))
            pNodeModel = g_GoalFactory.GetGoal((GoalType)planStepId, false);
        else if(BELONG(ActionType, planStepId))
            pNodeModel = g_ActionFactory.GetAction((ActionType)planStepId, false);
        else
            throw NotImplementedException(XcptHere);

        _ASSERTE(planStepId != NULL);

        NodeID nodeId = m_pGraph->AddNode(pNodeModel, pNodeModel->Id());

        GraphNodeView *pNodeView = new GraphNodeView(pNodeModel,
            (m_pPlanContext != nullptr ? &m_pPlanContext->Data.at(nodeId) : nullptr),
            m_pNodeMenu, nullptr);

        pNodeView->setPos(m_lastCtxtMenuScreenPos.x(), m_lastCtxtMenuScreenPos.y());

        m_nodeIdToNodeViewMap[nodeId] = pNodeView;

        addItem(pNodeView);
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::NodeSelected()
{
    GraphNodeView* pNodeView =  NULL;

    if(!selectedItems().empty())
        pNodeView = dynamic_cast<GraphNodeView*>(selectedItems().first());

    emit NodeSelected(pNodeView);
}
//----------------------------------------------------------------------------------------------
void GraphScene::DeleteNode()
{
    if(selectedItems().empty())
        return;

    GraphNodeView* pNodeView = NULL;

    foreach(QGraphicsItem *pItem, selectedItems())
    {
        pNodeView = dynamic_cast<GraphNodeView*>(pItem);

        if(pNodeView == NULL)
            continue;

        m_pGraph->RemoveNode(pNodeView->ModelId());

        QList<GraphEdgeView*> disconnectedEdges = pNodeView->Disconnect();

        foreach(GraphEdgeView* pEdge, disconnectedEdges)
            removeItem(pEdge);

        removeItem(pItem);
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::DeleteEdge()
{
    if(selectedItems().empty())
        return;

    GraphEdgeView* edgeItem = NULL;
    foreach(QGraphicsItem *item, selectedItems())
    {
        edgeItem = dynamic_cast<GraphEdgeView*>(item);

        if(edgeItem == NULL)
            continue;

        NodeID startNode, endNode;
        startNode = edgeItem->StartNode()->ModelId();
        endNode = edgeItem->EndNode()->ModelId();

        m_pGraph->RemoveEdge(startNode, endNode);

        removeItem(item);
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::DuplicateNode()
{
    if(selectedItems().empty())
        return;

    GraphNodeView* nodeItem = NULL;

    foreach(QGraphicsItem *item, selectedItems())
    {
        nodeItem = dynamic_cast<GraphNodeView*>(item);

        if(nodeItem == NULL)
            continue;

        NodeValue sourceNode = (NodeValue)m_pGraph->GetNode(nodeItem->ModelId())->Clone();
        m_pGraph->AddNode(sourceNode, sourceNode->Id());
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::ConnectNodeViews(GraphNodeView* pStart, GraphNodeView* pEnd)
{
    if(pStart != NULL && pEnd != NULL)
    {
        GraphEdgeView* pEdge = new GraphEdgeView(pStart, pEnd, m_pEdgeMenu, nullptr);
        pStart->AddEdge(pEdge);
        pEnd->AddEdge(pEdge);
        addItem(pEdge);
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::DisconnectNode()
{
    if(selectedItems().empty())
        return;

    GraphNodeView* pNodeView = NULL;

    foreach(QGraphicsItem *pItem, selectedItems())
    {
        pNodeView = dynamic_cast<GraphNodeView*>(pItem);

        if(pNodeView == NULL)
            continue;

        QList<GraphEdgeView*> disconnectedEdges = pNodeView->Disconnect();

        foreach(GraphEdgeView* pEdge, disconnectedEdges)
        {
            m_pGraph->RemoveEdge(pEdge->StartNode()->ModelId(), pEdge->EndNode()->ModelId());
            removeItem(pEdge);
        }
    }
}
//----------------------------------------------------------------------------------------------
bool GraphScene::event(QEvent * pEvt)
{
    _ASSERTE(pEvt);
    SceneEvent evt = (SceneEvent)pEvt->type();

    if (evt == SCENEEVT_GraphStructureChange)
    {
        OnGraphStructureChange();
        return true;
    }
    else if (evt == SCENEEVT_GraphRedraw)
    {
        OnGraphUpdate();
        return true;
    }
    else
    {
        return QGraphicsScene::event(pEvt);
    }
}
//----------------------------------------------------------------------------------------------
void GraphScene::OnGraphUpdate()
{
    if (m_pGraph != nullptr && m_pPlanContext != nullptr)
    {
        m_pGraph->Lock();

        for (auto& record : m_nodeIdToNodeViewMap)
        {
            if (m_pPlanContext->ActiveGoalSet.count(record.first) > 0)
                record.second->MarkAsActive();
            else
                record.second->MarkAsInactive();

            if (m_pGraph->Contains(record.first))
                record.second->OnUpdate();
        }

        m_pGraph->Unlock();
    }

    update();
}

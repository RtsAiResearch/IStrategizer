#ifndef GRAPHSCENE_H
#include "GraphScene.h"
#endif
#ifndef PLANGRAPH_H
#include "PlanGraph.h"
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


#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QAction>
#include <deque>
#include <set>

GraphScene::GraphScene(CrossMap<unsigned, string>* p_idLookup, QObject *p_parent) : QGraphicsScene(p_parent)
{
	m_cellSize				= DefaultCellSize;
	m_nodeHeight			= DefaultNodeHeight;
	m_nodeWidth				= DefaultNodeWidth;
	m_horizontalNodeSpacing = DefaultHorizontalNodeSpacing;
	m_verticalNodeSpacing	= DefaultVerticalNodeSpacing;
	m_nodeMenu				= NULL;
	m_planGraph				= NULL;
	m_choosePlanStep		= new ChoosePlanStepDialog(p_idLookup, true, true);
	m_mode					= MODE_Move;
	m_connectionLine		= NULL;

	connect(this, SIGNAL(selectionChanged()), SLOT(NodeSelected()));

	CreateMenus();
	UpdateView();
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateMenus()
{
	CreateNodeMenu();
	CreateEdgeMenu();
	CreateSceneMenu();
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateNodeMenu()
{
	QAction *duplicateAction = new QAction(tr("Duplicate"), this);
	duplicateAction->setStatusTip(tr("Duplicates graph node"));
	connect(duplicateAction, SIGNAL(triggered()), this, SLOT(DuplicateNode()));

	QAction *deleteAction = new QAction(tr("Delete"), this);
	duplicateAction->setStatusTip(tr("Deletes graph node"));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(DeleteNode()));

	QAction *disconnectAction = new QAction(tr("Disconnect"), this);
	disconnectAction->setStatusTip(tr("Disconnects the node from the graph"));
	connect(disconnectAction, SIGNAL(triggered()), this, SLOT(DisconnectNode()));

	m_nodeMenu = new QMenu(tr("Node Menu"));
	m_nodeMenu->setTearOffEnabled(true);
	m_nodeMenu->addAction(deleteAction);
	m_nodeMenu->addAction(duplicateAction);
	m_nodeMenu->addAction(disconnectAction);
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateEdgeMenu()
{
	QAction *deleteAction = new QAction(tr("&Delete"), this);
	deleteAction->setStatusTip(tr("Deletes graph edge"));
	deleteAction->setShortcut(QString::fromAscii("D"));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(DeleteEdge()));

	m_edgeMenu = new QMenu(tr("Edge Menu"));
	m_edgeMenu->setTearOffEnabled(true);
	m_edgeMenu->addAction(deleteAction);
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateSceneMenu()
{
	QAction *newNodeAction = new QAction(tr("N&ew Node"), this);
	newNodeAction->setStatusTip(tr("Creates new plan graph node"));
	connect(newNodeAction, SIGNAL(triggered()), this, SLOT(NewNode()));

	QAction *layoutGraph = new QAction(tr("L&ayout Plan"), this);
	layoutGraph->setStatusTip(tr("Layout plan graph in the scene"));
	connect(layoutGraph, SIGNAL(triggered()), this, SLOT(UpdateView()));

	m_sceneMenu = new QMenu(tr("Scene Menu"));
	m_sceneMenu->setTearOffEnabled(true);
	m_sceneMenu->addAction(newNodeAction);
	m_sceneMenu->addAction(layoutGraph);
}
//----------------------------------------------------------------------------------------------
void GraphScene::View(GoalEx* p_caseGoal, PlanGraph* p_planGraph)
{
	m_planGraph = p_planGraph;
	m_caseGoal = p_caseGoal;

	UpdateView();
}
//----------------------------------------------------------------------------------------------
void GraphScene::UpdateView()
{
	//CreateGrid();
	ConstructGraph();
	LayoutGraph();
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateGrid()
{
	int height = this->height(), width = this->width();

	QPen p(Qt::DotLine);
	for(int y = 0; y < height - 1; y += m_cellSize)
	{
		addLine(0, y, width, y, p);
	}

	for(int x = 0; x < width - 1; x += m_cellSize)
	{	
		addLine(x, 0, x, height, p);
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::ConstructGraph()
{
	if(m_planGraph == NULL)
	{
		clear();	
		return;
	}

	vector<int>				roots = m_planGraph->GetRoots();
	deque< pair<int,int> >	Q;
	set<int>				visitedNodes;

	for(int i = 0; i < roots.size(); ++i)
	{
		Q.push_back(make_pair(roots[i], 1));
		visitedNodes.insert(roots[i]);
	}

	for(int i = 0; i < m_graphLevels.size(); ++i)
		m_graphLevels[i].clear();
	m_graphLevels.clear();

	while(!Q.empty())
	{
		int nodeIndex = Q.front().first;
		int nodeLevel = Q.front().second;
		
		Q.pop_front();
		if(nodeLevel >= m_graphLevels.size())
			m_graphLevels.resize(nodeLevel + 1);

		m_graphLevels[nodeLevel].push_back(nodeIndex);
		vector<int> children = m_planGraph->GetChildren(nodeIndex);

		for (int i = 0; i < children.size(); ++i)
		{
			if(visitedNodes.find(children[i]) == visitedNodes.end())
			{
				visitedNodes.insert(children[i]);
				Q.push_back(make_pair(children[i], nodeLevel + 1));
			}
		}
	}

	m_nodeModelViewMapping.clear();
	m_nodeIndexViewMapping.clear();
	clear();

	GraphNodeView*				nodeView	= NULL;
	PlanStepEx*					nodeModel	= NULL;

	for (int i = 0 ; i < m_graphLevels.size(); ++i)
	{
		for (int j = 0 ; j < m_graphLevels[i].size(); ++j)
		{
			nodeModel = m_planGraph->operator [](m_graphLevels[i][j])->Value();

			int nodeIndex = m_graphLevels[i][j];
			nodeView = new GraphNodeView(nodeModel, nodeIndex, m_nodeMenu, NULL, this);

			m_nodeIndexViewMapping[nodeIndex] = nodeView;
			m_nodeModelViewMapping[nodeModel] = nodeView;

			nodeView->setRect(0, 0, m_nodeWidth, m_nodeHeight);

			addItem(nodeView);
		}
	}

	ConnectGraphNodes();
	CreateCaseGoalNode(roots, m_nodeIndexViewMapping);
}
//----------------------------------------------------------------------------------------------
void GraphScene::CreateCaseGoalNode( vector<int> &p_roots, map<int, GraphNodeView*> &p_nodeIndexViewMapping )
{
	if(m_caseGoal == NULL)
		return;

	GraphNodeView* caseGoalNodeView = new GraphNodeView(m_caseGoal, CaseGoalNodeIndex, NULL, NULL, this); 
	m_nodeIndexViewMapping[CaseGoalNodeIndex] = caseGoalNodeView;

	caseGoalNodeView->setRect(0, 0, m_nodeWidth, m_nodeHeight);
	addItem(caseGoalNodeView);

	for(int j = 0; j < p_roots.size(); ++j)
	{
		GraphNodeView* start = caseGoalNodeView;
		GraphNodeView* end = p_nodeIndexViewMapping[p_roots[j]];
		GraphEdgeView* edge = new GraphEdgeView(start, end, m_edgeMenu, NULL, this);
		start->AddEdge(edge);
		end->AddEdge(edge);
		addItem(edge);
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::UpdateNodesIndices()
{
	for(int i = 0; i < m_planGraph->Size(); ++i)
	{
		GraphNode<PlanStepEx*, EdgeAnnotation>* node = m_planGraph->operator [](i);
		m_nodeModelViewMapping[node->Value()]->Index(i);
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::LayoutGraph()
{
	if(m_planGraph == NULL)
		return;

	GraphNodeView*	nodeView	= NULL;
	PlanStepEx*		nodeModel	= NULL;
	int x, y;
	int nodeIndex, levelWidth, leveStartlX;
	for (int i = 0 ; i < m_graphLevels.size(); ++i)
	{
		y = m_verticalNodeSpacing + (i * (m_nodeHeight + m_verticalNodeSpacing));
		levelWidth = (m_graphLevels[i].size() * m_nodeWidth) + ((m_graphLevels[i].size() - 1) * m_horizontalNodeSpacing);
		leveStartlX = (width() - levelWidth) / 2.0;

		for (int j = 0 ; j < m_graphLevels[i].size(); ++j)
		{
			x = leveStartlX + (j  * (m_nodeWidth + m_horizontalNodeSpacing));
			nodeIndex = m_graphLevels[i][j];
			m_nodeIndexViewMapping[nodeIndex]->setPos(QPointF(x, y));
		}
	}

	if(!m_nodeIndexViewMapping.empty())
	{
		y = m_verticalNodeSpacing;
		x = (width() - m_nodeWidth) / 2.0;
		m_nodeIndexViewMapping[CaseGoalNodeIndex]->setPos(QPointF(x, y));
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::ConnectGraphNodes()
{
	if(m_planGraph == NULL)
		return;

	for(int i = 0; i < m_planGraph->Size(); ++i)
	{
		vector<int> children;
		GraphNode<PlanStepEx*, EdgeAnnotation>* node = m_planGraph->operator [](i);
		children = node->GetChildren();

		for(int j = 0; j < children.size(); ++j)
		{
			GraphNodeView* start = m_nodeIndexViewMapping[i];
			GraphNodeView* end = m_nodeIndexViewMapping[children[j]];
			GraphEdgeView* edge = new GraphEdgeView(start, end, m_edgeMenu, NULL, this);
			start->AddEdge(edge);
			end->AddEdge(edge);
			addItem(edge);
		}
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *p_mouseEvent)
{
	if (p_mouseEvent->button() != Qt::LeftButton)
		return;

	if(m_mode == MODE_Connect)
	{
		m_connectionLine = new QGraphicsLineItem(QLineF(p_mouseEvent->scenePos(), p_mouseEvent->scenePos()));
		m_connectionLine->setPen(QPen(QColor("black"), 2));
		addItem(m_connectionLine);
	}

	QGraphicsScene::mousePressEvent(p_mouseEvent);
}
//----------------------------------------------------------------------------------------------
void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *p_mouseEvent)
{
	if (m_mode == MODE_Connect && m_connectionLine != 0) 
	{
		QLineF newLine(m_connectionLine->line().p1(), p_mouseEvent->scenePos());
		m_connectionLine->setLine(newLine);
	} 
	else if (m_mode == MODE_Move) 
	{
		QGraphicsScene::mouseMoveEvent(p_mouseEvent);
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *p_mouseEvent)
{
	if (m_connectionLine != 0 && m_mode == MODE_Connect) 
	{
		QList<QGraphicsItem *> startItems = items(m_connectionLine->line().p1());
		if (startItems.count() && startItems.first() == m_connectionLine)
			startItems.removeFirst();

		QList<QGraphicsItem *> endItems = items(m_connectionLine->line().p2());
		if (endItems.count() && endItems.first() == m_connectionLine)
			endItems.removeFirst();

		removeItem(m_connectionLine);
		delete m_connectionLine;

		if (startItems.count() > 0 && endItems.count() > 0 &&
			startItems.first() != endItems.first()) 
		{
			GraphNodeView *startNode = qgraphicsitem_cast<GraphNodeView *>(startItems.first());
			GraphNodeView *endNode = qgraphicsitem_cast<GraphNodeView *>(endItems.first());
			ConnectNodes(startNode, endNode);
			UpdateView();
		}
	}

	m_connectionLine = NULL;
	QGraphicsScene::mouseReleaseEvent(p_mouseEvent);
}
//----------------------------------------------------------------------------------------------
void GraphScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	event->setAccepted(false);

	QGraphicsScene::contextMenuEvent(event);

	if(!event->isAccepted())
	{
		event->setAccepted(true);
		if(m_sceneMenu != NULL)
			m_sceneMenu->exec(event->screenPos());
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::NewNode()
{
	if(m_choosePlanStep->exec() == QDialog::Accepted)
	{
		unsigned planStepId = m_choosePlanStep->SelectedPlanStepId();
		PlanStepEx* planStep = NULL;

		if(BELONG(GoalType, planStepId))
			planStep = g_GoalFactory.GetGoal((GoalType)planStepId, false);
		else if(BELONG(ActionType, planStepId))
			planStep = g_ActionFactory.GetAction((ActionType)planStepId, false);

		assert(planStepId != NULL);
		m_planGraph->AddNode(planStep, m_planGraph->Size());
		UpdateView();
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::NodeSelected()
{
	GraphNodeView* node =  NULL;

	if(!selectedItems().empty())
		node = dynamic_cast<GraphNodeView*>(selectedItems().first());

	//if(node != NULL)
	emit NodeSelected(node);
}
//----------------------------------------------------------------------------------------------
void GraphScene::DeleteNode()
{
	if(selectedItems().empty())
		return;

	GraphNodeView* nodeItem = NULL;
	foreach(QGraphicsItem *item, selectedItems())
	{
		nodeItem = dynamic_cast<GraphNodeView*>(item);

		if(nodeItem == NULL)
			continue;

		if(nodeItem->Index() == CaseGoalNodeIndex)
			continue;

		GraphNode<PlanStepEx*, EdgeAnnotation>* node = m_planGraph->operator [](nodeItem->Index());
		m_nodeModelViewMapping.erase(node->Value());
		m_planGraph->RemoveNode(nodeItem->Index());
		UpdateNodesIndices();

		QList<GraphEdgeView*> disconnectedEdges = nodeItem->Disconnect();

		foreach(GraphEdgeView* edge, disconnectedEdges)
			removeItem(edge);

		removeItem(item);
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

		int start, end;
		start	= edgeItem->StartNode()->Index();
		end		= edgeItem->EndNode()->Index();

		if(start != CaseGoalNodeIndex && end != CaseGoalNodeIndex)
			m_planGraph->Disconnect(start, end);

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

		PlanStepEx* sourceNode = m_planGraph->operator [](nodeItem->Index())->Value();
		m_planGraph->AddNode((PlanStepEx*)sourceNode->Clone(), m_planGraph->Size());
		//m_planGraph->AddNode(newNode->Clone(), m_planGraph->Size());
	}
}
//----------------------------------------------------------------------------------------------
void GraphScene::ConnectNodes(GraphNodeView* p_start, GraphNodeView* p_end)
{
	if(p_start != NULL && p_end != NULL && p_start->Index() != CaseGoalNodeIndex && p_end->Index() != CaseGoalNodeIndex)
		m_planGraph->Connect(p_start->Index(), p_end->Index(), SVector<Expression*>());
}
//----------------------------------------------------------------------------------------------
void GraphScene::DisconnectNode()
{
	if(selectedItems().empty())
		return;

	GraphNodeView* nodeItem = NULL;
	foreach(QGraphicsItem *item, selectedItems())
	{
		nodeItem = dynamic_cast<GraphNodeView*>(item);

		if(nodeItem == NULL)
			continue;

		if(nodeItem->Index() == CaseGoalNodeIndex)
			continue;

		GraphNode<PlanStepEx*, EdgeAnnotation>* node = m_planGraph->operator [](nodeItem->Index());
		QList<GraphEdgeView*> disconnectedEdges = nodeItem->Disconnect();
		
		foreach(GraphEdgeView* edge, disconnectedEdges)
		{
			if (edge->StartNode()->Index() == CaseGoalNodeIndex ||
				edge->EndNode()->Index() == CaseGoalNodeIndex)
				continue;

			m_planGraph->Disconnect(edge->StartNode()->Index(), edge->EndNode()->Index());
			removeItem(edge);
		}
	}
}
//----------------------------------------------------------------------------------------------
GraphScene::~GraphScene()
{

}

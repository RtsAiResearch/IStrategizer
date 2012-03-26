#ifndef GRAPHNODEVIEW_H
#include "GraphNodeView.h"
#endif
#ifndef GRAPHEDGEVIEW_H
#include "GraphEdgeView.h"
#endif
#ifndef PLANSTEPEX_H
#include "PlanStepEx.h"
#endif

#include <algorithm>
#include <QBrush>
#include <sstream>
#include <QColor>
#include <QTextOption>
#include <QTextItem>
#include <QFont>
#include <QMenu>
#include <QList>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>

using namespace MetaData;

GraphNodeView::GraphNodeView(PlanStepEx* p_planStep, int p_index, QMenu *p_contextMeun, QGraphicsItem *p_parent /* = 0 */, QGraphicsScene *p_scene /* = 0 */) : QGraphicsRectItem(p_parent, p_scene)
{
	m_nodeModel		= p_planStep;
	m_nodeHeight	= DefaultNodeHeight;
	m_nodeWidth		= DefaultNodeWidth;
	m_contextMenu	= p_contextMeun;
	m_index			= p_index;

	if(m_index == CaseGoalNodeIndex)
	{
		QColor color(255, 0, 0, 192);
		QBrush brush(color, Qt::SolidPattern);
		setBrush(brush);
	}
	else if(BELONG(GoalType, p_planStep->StepTypeId()))
	{
		QColor color(0, 0, 255, 192);
		QBrush brush(color, Qt::SolidPattern);
		setBrush(brush);
	}
	else
	{
		QColor color(0, 255, 0, 192);
		QBrush brush(color, Qt::SolidPattern);
		setBrush(brush);
	}

	setToolTip(QString::fromStdString(p_planStep->TypeName()));
	setRect(0, 0, m_nodeWidth, m_nodeHeight);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
}
//----------------------------------------------------------------------------------------------
void GraphNodeView::AddEdge(GraphEdgeView* p_edge)
{
	m_edges.append(p_edge);
}
//----------------------------------------------------------------------------------------------
void GraphNodeView::RemoveEdge(GraphEdgeView* p_edge)
{
	QList<GraphEdgeView*>::iterator where = find(m_edges.begin(), m_edges.end(), p_edge);

	if(where != m_edges.end())
		m_edges.erase(where);
}
//----------------------------------------------------------------------------------------------
QList<GraphEdgeView*> GraphNodeView::Disconnect()
{
	GraphNodeView *other = NULL;
	QList<GraphEdgeView*> edges(m_edges);

	foreach(GraphEdgeView* edge, edges)
	{
		if(edge->StartNode() != this)
			other = edge->StartNode();
		else
			other = edge->EndNode();

		other->RemoveEdge(edge);
		this->RemoveEdge(edge);
	}

	return edges;
}
//----------------------------------------------------------------------------------------------
QVariant GraphNodeView::itemChange(GraphicsItemChange change,
								 const QVariant &value)
{
	if (change == QGraphicsItem::ItemPositionChange) 
	{
		foreach (GraphEdgeView *edge, m_edges) 
		{
			edge->UpdatePosition();
		}
	}

	return value;
}
//----------------------------------------------------------------------------------------------
void GraphNodeView::paint(QPainter *p_painter, const QStyleOptionGraphicsItem *p_option, QWidget *p_widget)
{ 
	QGraphicsRectItem::paint(p_painter, p_option, p_widget);
	QString txt;
	if(BELONG(GoalType, m_nodeModel->StepTypeId()))
		txt = "G";
	else
		txt = "A";

	QTextOption textOption;
	textOption.setAlignment(Qt::AlignCenter);
	QFont font;
	font.setBold(true);
	font.setPixelSize(20);
	p_painter->setFont(font);
	p_painter->drawText(this->rect(), txt, textOption);
}
//----------------------------------------------------------------------------------------------
void GraphNodeView::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	event->accept();
	scene()->clearSelection();
	setSelected(true);
	if(m_contextMenu != NULL)
		m_contextMenu->exec(event->screenPos());
}
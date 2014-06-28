#include "stdafx.h"

#ifndef GRAPHNODEVIEW_H
#include "GraphNodeView.h"
#endif
#ifndef GRAPHEDGEVIEW_H
#include "stdafx.h"
#include "GraphEdgeView.h"
#endif
#ifndef PLANSTEPEX_H
#include "PlanStepEx.h"
#endif

using namespace IStrategizer;
using namespace std;

GraphNodeView::GraphNodeView(PlanStepEx* pPlanStep, NodeID modelId, QMenu *pContextMeun, QGraphicsItem *pParent /* = 0 */) 
    : QGraphicsRectItem(pParent),
    m_modelId(modelId)
{
    m_pNodeModel = pPlanStep;
    m_pContextMenu = pContextMeun;

    //string nodeName = m_pNodeModel->ToString();
    //unsigned findLeftSqrParnPos = nodeName.find("[");

    //if (findLeftSqrParnPos != string::npos)
    //{
    //    nodeName = nodeName.substr(0, findLeftSqrParnPos);
    //}
    //nodeName += '[';
    //nodeName += to_string(modelId);
    //nodeName += ']';

    m_nodeTxt = QString::fromStdString(pPlanStep->ToString(true));

    m_style = GetStyle();

    setBrush(m_style.BackgroundBrush);
    setPen(m_style.BorderPen);

    // Measure text width and set the node width accordingly with small padding
    QFontMetrics fontMetric(m_style.TxtFont);

    m_nodeHeight    = fontMetric.height() + 40;
    m_nodeWidth        = fontMetric.width(m_nodeTxt) + 20;

    setToolTip(QString::fromLocal8Bit(pPlanStep->TypeName().c_str()));
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
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
    
    m_style = GetStyle();

    setBrush(m_style.BackgroundBrush);
    setPen(m_style.BorderPen);
    
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);

    p_painter->setFont(m_style.TxtFont);
    p_painter->setPen(m_style.TxtPen);

        // Measure text width and set the node width accordingly with small padding
    QFontMetrics fontMetric(m_style.TxtFont);

    m_nodeHeight    = fontMetric.height() + 20;
    m_nodeWidth        = fontMetric.width(m_nodeTxt) + 20;

    // Call setRect ONLY in case the node dimensions changed, if it is called each paint
    // it will cause infinite loop (and lead to stack overflow) because setRect always
    // case the node to repaint
    if (rect().width() != m_nodeWidth || rect().height() != m_nodeHeight)
        setRect(0, 0, m_nodeWidth, m_nodeHeight);

    p_painter->drawText(this->rect(), m_nodeTxt, textOption);
}
//----------------------------------------------------------------------------------------------
void GraphNodeView::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    event->accept();
    scene()->clearSelection();
    setSelected(true);
    if(m_pContextMenu != NULL)
        m_pContextMenu->exec(event->screenPos());
}
//////////////////////////////////////////////////////////////////////////
GraphNodeView::NodeStyle GraphNodeView::GetStyle()
{
    NodeStyle style;

    Qt::GlobalColor bgBrushColor;
    Qt::GlobalColor txtPenColor = Qt::black;
    Qt::GlobalColor borderColor = Qt::black;

    switch (m_pNodeModel->State())
    {
    case ESTATE_Failed:
        bgBrushColor = Qt::red;
        break;
    case ESTATE_Succeeded:
        bgBrushColor = Qt::green;
        break;
    case ESTATE_NotPrepared:
        bgBrushColor = Qt::lightGray;
        break;
    case ESTATE_Pending:
        bgBrushColor = Qt::yellow;
        break;
    case ESTATE_Executing:
        bgBrushColor = Qt::blue;
        txtPenColor = Qt::white;
        break;
    default:
        bgBrushColor = Qt::white;
        break;
    }

    style.BackgroundBrush = QBrush(bgBrushColor, Qt::SolidPattern);
    style.BorderPen = QPen(QColor(borderColor));
    style.TxtPen = QPen(QColor(txtPenColor));

    if (BELONG(GoalType, m_pNodeModel->StepTypeId()))
    {
        style.TxtFont.setBold(true);
        style.TxtFont.setPixelSize(26);
        style.BorderPen.setWidth(2);
    }
    else
    {
        //style.TxtFont.setBold(true);
        style.TxtFont.setPixelSize(22);
        style.BorderPen.setWidth(1);
    }

    return style;
} 

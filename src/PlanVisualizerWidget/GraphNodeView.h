#ifndef GRAPHNODEVIEW_H
#define GRAPHNODEVIEW_H

#pragma warning(push, 3)
#include <QGraphicsRectItem>
#include <QList>
#include <QPen>
#include <QFont>
#include <QBrush>
#pragma warning(pop)

#include "EngineData.h"

class QGraphicsItem;
class QGraphicsScene;
class QMenu;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;

namespace IStrategizer
{
    class GraphEdgeView;
    class PlanStepEx;

    class GraphNodeView : public QGraphicsRectItem
    {
    public:
        typedef IOlcbpPlan::NodeID NodeID;

        struct NodeStyle
        {
            QBrush BackgroundBrush;
            QPen BorderPen;
            QPen TxtPen;
            QFont TxtFont;
        };

        GraphNodeView(PlanStepEx* pPlanStep, NodeID modelId, QMenu *pContextMenu, QGraphicsItem *pParent = 0);
        void RemoveEdge(GraphEdgeView* pEdge);
        QList<GraphEdgeView*> Disconnect();
        PlanStepEx* NodeModel() { return m_pNodeModel; }
        const PlanStepEx* NodeModel() const { return m_pNodeModel; }
        void AddEdge(GraphEdgeView* pEdge);
        int ModelId() const { return m_modelId; }
        int NodeWidth() const { return m_nodeWidth; }
        int NodeHeight() const { return m_nodeHeight; }

    protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);
        void paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget = 0);
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *pEvt);
        NodeStyle GetStyle();

    private:
        int m_nodeWidth;
        int m_nodeHeight;
        QMenu *m_pContextMenu;
        NodeID m_modelId;
        PlanStepEx* m_pNodeModel;
        QString m_nodeTxt;
        QList<GraphEdgeView*> m_edges;
        NodeStyle m_style;
    };
}

#endif // GRAPHNODEVIEW_H
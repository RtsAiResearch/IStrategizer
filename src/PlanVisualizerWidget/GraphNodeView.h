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
#include "OlcbpPlanNodeData.h"

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
		struct NodeStyle
		{
			QBrush BackgroundBrush;
			QPen BorderPen;
			QPen TxtPen;
			QFont TxtFont;
		};

		GraphNodeView(PlanStepEx* pPlanStep, ConstOlcbpPlanNodeDataPtr pNodeData, QMenu *pContextMenu, QGraphicsItem *pParent = 0);
		void RemoveEdge(GraphEdgeView* pEdge);
		QList<GraphEdgeView*> Disconnect();
		PlanStepEx* NodeModel() { return m_pNodeModel; }
		const PlanStepEx* NodeModel() const { return m_pNodeModel; }
		IOlcbpPlan::NodeID SatisfyingGoalId() const { return m_SatisfyingGoalId; }
		void AddEdge(GraphEdgeView* pEdge);
		IOlcbpPlan::NodeID ModelId() const { return m_modelId; }
		int NodeWidth() const { return m_nodeWidth; }
		int NodeHeight() const { return m_nodeHeight; }
		void MarkAsActive() { m_isActive = true; }
		void MarkAsInactive() { m_isActive = false; }
		void OnUpdate();

	protected:
		QVariant itemChange(GraphicsItemChange change, const QVariant &value);
		void paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget = 0);
		void contextMenuEvent(QGraphicsSceneContextMenuEvent *pEvt);
		NodeStyle GetStyle();

	private:
		QMenu *m_pContextMenu;
		QString m_nodeTxt;
		QList<GraphEdgeView*> m_edges;
		NodeStyle m_style;

		PlanStepEx* m_pNodeModel;
		ConstOlcbpPlanNodeDataPtr m_pNodeData;

		// Properties
		bool m_isGoal;
		int m_nodeWidth;
		int m_nodeHeight;
		IOlcbpPlan::NodeID m_modelId;
		IOlcbpPlan::NodeID m_SatisfyingGoalId;

		// State
		bool m_isActive;
		bool m_isOpen;
		ExecutionStateType m_state;
		char m_tooltipTxt[512];
		std::string m_nodeToString;
	};
}

#endif // GRAPHNODEVIEW_H
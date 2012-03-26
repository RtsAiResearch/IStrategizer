#ifndef GRAPHNODEVIEW_H
#define GRAPHNODEVIEW_H

#include <QGraphicsRectItem>
#include <QList>

class QGraphicsItem;
class QGraphicsScene;
class GraphEdgeView;
class PlanStepEx;
class QMenu;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;

const int DefaultNodeWidth = 48;
const int DefaultNodeHeight = 32;
const int CaseGoalNodeIndex = -1;

class GraphNodeView : public QGraphicsRectItem
{
public:
	GraphNodeView(PlanStepEx* p_planStep, int p_index, QMenu *p_contextMenu, QGraphicsItem *p_parent = 0, QGraphicsScene *p_scene = 0);
	void RemoveEdge(GraphEdgeView* p_edge);
	QList<GraphEdgeView*> Disconnect();
	PlanStepEx* NodeModel() const { return m_nodeModel; }
	void AddEdge(GraphEdgeView* p_edge);
	int Index() const { return m_index; }
	void Index(int p_index) { m_index = p_index; }

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void paint(QPainter *p_painter, const QStyleOptionGraphicsItem *p_option, QWidget *p_widget = 0);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
	int m_nodeWidth;
	int m_nodeHeight;
	QMenu *m_contextMenu;
	int m_index;
	PlanStepEx* m_nodeModel;
	QList<GraphEdgeView*> m_edges;

};


#endif // GRAPHNODEVIEW_H
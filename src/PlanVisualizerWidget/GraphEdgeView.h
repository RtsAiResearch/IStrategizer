#ifndef GRAPHEDGEVIEW_H
#define GRAPHEDGEVIEW_H

#pragma warning(push, 3)
#include <QGraphicsLineItem>
#include <QPolygonF>
#include <QPainterPath>
#pragma warning(pop)

class QGraphicsItem;
class QGraphicsScene;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QGraphicsSceneContextMenuEvent;
class QMenu;

namespace IStrategizer
{
    class GraphNodeView;
    const int DefaultArrowSize = 15;
    
    class GraphEdgeView : public QGraphicsLineItem
    {
    public:
        GraphEdgeView(GraphNodeView *p_startNode, GraphNodeView *p_endNode,
            QMenu* m_contextMenu = NULL, QGraphicsItem *p_parent = 0);

        QRectF boundingRect() const;
        QPainterPath shape() const;

        GraphNodeView *StartNode() const { return m_startNode; }
        GraphNodeView *EndNode() const { return m_endNode; }

        public slots:
            void UpdatePosition();

    protected:
        void paint(QPainter *p_painter, const QStyleOptionGraphicsItem *p_option, QWidget *p_widget = 0);
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    private:
        QMenu *m_contextMenu;
        GraphNodeView* m_startNode;
        GraphNodeView* m_endNode;
        QPolygonF m_arrowHead;
        double m_arrowSize;
        QColor m_color;
        QColor m_selectedColor;
    };
}

#endif // GRAPHEDGEVIEW_H

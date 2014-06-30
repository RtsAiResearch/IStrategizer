#include "stdafx.h"
#ifndef GRAPHNODEVIEW_H
#include "GraphNodeView.h"
#endif
#ifndef GRAPHEDGEVIEW_H
#include "GraphEdgeView.h"
#endif


using namespace std;
using namespace IStrategizer;

const qreal Pi = acos(-1.0);

GraphEdgeView::GraphEdgeView(GraphNodeView *p_startNode, GraphNodeView *p_endNode,
          QMenu* p_contextMenu, QGraphicsItem *p_parent)
          : QGraphicsLineItem(p_parent)
{
    m_startNode = p_startNode;
    m_endNode = p_endNode;
    m_color = Qt::black;
    m_selectedColor = Qt::red;
    m_arrowSize = DefaultArrowSize;
    m_contextMenu = p_contextMenu;

    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::SquareCap));
    setFlag(ItemIsSelectable, true);

    setCacheMode(QGraphicsItem::ItemCoordinateCache);
}
//----------------------------------------------------------------------------------------------
QRectF GraphEdgeView::boundingRect() const
{
    qreal extra = (pen().width() + 20);

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
        line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//----------------------------------------------------------------------------------------------
QPainterPath GraphEdgeView::shape() const
{
    QPainterPath path;// = QGraphicsLineItem::shape();

    QLineF normal = this->line().unitVector().normalVector();
    qreal dx = normal.dx();
    qreal dy = normal.dy();

    QLineF myLine;

    myLine = this->line();
    myLine.translate(dx * 4, dy * 4);
    path.lineTo(myLine.p1());
    path.lineTo(myLine.p2());

    myLine = this->line();
    myLine.translate(-dx * 4,-dy * 4);
    path.lineTo(myLine.p2());
    path.lineTo(myLine.p1());
    path.closeSubpath();

    path.addPolygon(m_arrowHead);

    return path;
}
//----------------------------------------------------------------------------------------------
void GraphEdgeView::UpdatePosition()
{
    update();
}
//----------------------------------------------------------------------------------------------
void GraphEdgeView::paint(QPainter *p_painter, const QStyleOptionGraphicsItem *p_option, QWidget *p_widget)
{
    if (m_startNode->collidesWithItem(m_endNode))
        return;

    QPen myPen = pen();
    myPen.setColor(isSelected() ? m_selectedColor : m_color);
    qreal arrowSize = m_arrowSize;
    p_painter->setPen(myPen);
    p_painter->setBrush(isSelected() ? m_selectedColor : m_color);

    QLineF centerLine(m_startNode->mapToScene(m_startNode->rect().center()), m_endNode->mapToScene(m_endNode->rect().center()));
    QPolygonF endPolygon;
    QPointF p1;
    QPointF p2;
    QPointF intersectPoint1;
    QPointF intersectPoint2;
    QLineF polyLine;

    endPolygon = QPolygonF(m_endNode->rect());
    p1 = endPolygon.first() + m_endNode->pos();
    for (int i = 1; i < endPolygon.count(); ++i) 
    {
        p2 = endPolygon.at(i) + m_endNode->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType =
            polyLine.intersect(centerLine, &intersectPoint1);
        if (intersectType == QLineF::BoundedIntersection)
            break;
        p1 = p2;
    }

    endPolygon = QPolygonF(m_startNode->rect());
    p1 = endPolygon.first() + m_startNode->pos();
    for (int i = 1; i < endPolygon.count(); ++i) 
    {
        p2 = endPolygon.at(i) + m_startNode->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType =
            polyLine.intersect(centerLine, &intersectPoint2);
        if (intersectType == QLineF::BoundedIntersection)
            break;
        p1 = p2;
    }

    setLine(QLineF(intersectPoint1, intersectPoint2));

    double angle = ::acos(this->line().dx() / this->line().length());
    if (this->line().dy() >= 0)
        angle = (Pi * 2) - angle;

    QPointF arrowP1 = this->line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
        cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = this->line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
        cos(angle + Pi - Pi / 3) * arrowSize);

    m_arrowHead.clear();
    m_arrowHead << this->line().p1() << arrowP1 << arrowP2;
    p_painter->drawLine(this->line());
    p_painter->drawPolygon(m_arrowHead);

    //if (isSelected()) 
    //{
    //    p_painter->setPen(QPen(m_selectedColor, 2, Qt::DotLine));

    //    QLineF normal = this->line().unitVector().normalVector();
    //    qreal dx = normal.dx();
    //    qreal dy = normal.dy();

    //    QLineF myLine;
    //    
    //    myLine = this->line();
    //    myLine.translate(dx * 4, dy * 4);
    //    p_painter->drawLine(myLine);

    //    myLine = this->line();
    //    myLine.translate(-dx * 4,-dy * 4);
    //    p_painter->drawLine(myLine);
    //}
}
//----------------------------------------------------------------------------------------------
void GraphEdgeView::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    event->accept();
    scene()->clearSelection();
    setSelected(true);
    if(m_contextMenu != NULL)
        m_contextMenu->exec(event->screenPos());
}

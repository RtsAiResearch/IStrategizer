#ifndef GRAPHGRAPHICSVIEW_H
#define GRAPHGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

namespace IStrategizer
{
    class GraphGraphicsView : public QGraphicsView
    {
    public:
        GraphGraphicsView();

    protected:
        void wheelEvent(QWheelEvent* p_wheelEvent);
    };
}

#endif
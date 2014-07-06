#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#pragma warning(push, 3)
#include <QGraphicsScene>
#include <QEvent>
#pragma warning(pop)

#include <map>
#include <vector>

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif
#include "EngineData.h"
#include "OlcbpPlanNodeData.h"

class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QGraphicsLineItem;
class QTimer;

namespace IStrategizer
{
    class GraphNodeView;
    class GoalEx;
    class ChoosePlanStepDialog;

    const int DefaultGridCellSize = 16;
    const int DefaultHorizontalNodeSpacing = 48;
    const int DefaultVerticalNodeSpacing = 48;

    
    class GraphScene : public QGraphicsScene
    {
        Q_OBJECT

    public:
        enum SceneEvent
        {
            SCENEEVT_GraphStructureChange = (int)(QEvent::User) + 1,
            SCENEEVT_GraphRedraw,
        };

        class QGraphStructureChangeEvent : public QEvent
        {
        public:
            QGraphStructureChangeEvent(IOlcbpPlan* pPlan) :
                QEvent((QEvent::Type)SCENEEVT_GraphStructureChange),
                m_pPlan(pPlan)
            {
            }

            IOlcbpPlan* Plan() const { return m_pPlan; }

        protected:
            IOlcbpPlan* m_pPlan;
        };

        typedef IOlcbpPlan::NodeID NodeID;
        typedef IOlcbpPlan::NodeValue NodeValue;

        enum PointerMode { PTRMODE_Move, MODE_Connect };

        GraphScene(CrossMap<unsigned, std::string>* p_idLookup, QObject *p_parent = 0);
        void View(IOlcbpPlan* pGraph, ConstOlcbpPlanContextPtr pPlanContext);
        void Mode(PointerMode p_mode) { m_pointerMode = p_mode; }
        PointerMode Mode() const { return m_pointerMode; }

        //************************************
        // IStrategizer::GraphScene::OnGraphStructureChange
        // Description:    Called when the model graph nodes structure change, which gives the graph
        // scene a chance to redraw and layout the graph view
        // Returns:       void
        //************************************
        virtual void OnGraphStructureChange(IOlcbpPlan* pGraph);

        //************************************
        // IStrategizer::GraphScene::OnGraphUpdate
        // Description:	Called to redraw the current graph nodes without layouting the graph
        // Returns:   	void
        //************************************
        virtual void OnGraphUpdate();

    protected:
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *pEvent);
        void mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent);
        void mouseMoveEvent(QGraphicsSceneMouseEvent *pMouseEvent);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *pMouseEvent);
        bool event ( QEvent * eEvt );

    private:
        int m_cellSize;
        int m_horizontalNodeSpacing;
        int m_verticalNodeSpacing;
        GoalEx* m_caseGoal;
        QMenu* m_pNodeMenu;
        QMenu* m_pEdgeMenu;
        QMenu* m_pSceneMenu;
        QColor m_nodeColor;
        QColor m_lineColor;
        QPointF m_lastCtxtMenuScreenPos;
        QGraphicsLineItem* m_pConnectionLine;
        ChoosePlanStepDialog* m_pChoosePlanStepDlg;
        PointerMode m_pointerMode;
        IOlcbpPlan *m_pGraph;
        std::vector< std::vector<NodeID> > m_graphLevels;
        std::map<NodeID, GraphNodeView*> m_nodeIdToNodeViewMap;
        ConstOlcbpPlanContextPtr m_pPlanContext;

        void ConstructGraph();
        void ComputeGraphLevels();

        void CreateEdgeMenu();
        void CreateNodeMenu();
        void CreateSceneMenu();
        void UpdateNodesIndices();
        void ConnectNodeViews(GraphNodeView* pStartNode, GraphNodeView* pEndNode);
        void ConnectGraphNodes();
        void LayoutGraph();
        void LayoutGraphInHierarchy();
        int ComputeLevelWidth(int levelIdx);
        int ComputeLevelHeight(int levelIdx);

    private slots:
        void ReconstructScene();
        void NodeSelected();
        void NewNode();
        void DeleteNode();
        void DeleteEdge();
        void DisconnectNode();
        void DuplicateNode();

    signals:
        void NodeSelected(GraphNodeView* pNode);
    };
}

#endif // GRAPHSCENE_H
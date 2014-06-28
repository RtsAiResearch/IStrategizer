#ifndef PLANGRAPHVIEW_H
#define PLANGRAPHVIEW_H

#pragma warning(push, 3)
#include <QWidget>
#include "ui_PlanGraphView.h"
#pragma warning(pop)

#include <hash_map>
#include <string>
#include "CrossMap.h"
#include "GraphScene.h"
#include "MessagePump.h"
#include "OlcbpPlanNodeData.h"

class QGraphicsView;

namespace IStrategizer
{
    class GraphScene;
    class PlanStepView;
    class GraphNodeView;
    class GoalEx;

    const int GraphUpdateIntervalMs = 500;

    class PlanGraphView : public QWidget
    {
        Q_OBJECT

    public:
        PlanGraphView(GraphScene* pScene, CrossMap<unsigned, std::string>* pIdLookup, QWidget *pParent = 0);
        
        void View(IOlcbpPlan* pPlanGraph, ConstOlcbpPlanNodeDataMapPtr pNodeData = nullptr);
        void SetMode(GraphScene::PointerMode mode);
        void OnPlanStructureChange(IOlcbpPlan* pPlan);

    protected:
        void timerEvent(QTimerEvent *pEvt);

    private:
        Ui::PlanGraphViewClass m_ui;
        QGraphicsView* m_pGraphicsView;        
        GraphScene* m_pScene;
        PlanStepView* m_pPlanStepView;
        int m_updateTimerId;

        private slots:
            void HandleNodeSelected(GraphNodeView* pNode);
    };
}

#endif // PLANGRAPHVIEW_H

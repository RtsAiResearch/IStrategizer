#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include "ui_ClientMain.h"
#include "CrossMap.h"
#include "IMessagePumpObserver.h"
#include "EngineData.h"
#include <memory>
#include <vector>
#include "OlcbpPlanNodeData.h"
#include <QMainWindow>

namespace IStrategizer
{
    class RtsGame;
    class IStrategizerEx;
    class GameTraceCollector;
    class PlanGraphView;
}

class IMViewWidget;
class PlannerViewWidget;


class PlanSnapshot
{
public:
    PlanSnapshot(unsigned gameFrame,
        std::shared_ptr<IStrategizer::IOlcbpPlan> pPlan,
        IStrategizer::ConstOlcbpPlanNodeDataMapRef data,
        IStrategizer::IOlcbpPlan::NodeSet activeGoalSet) :
        m_context(m_data, m_activeGoalSet),
        m_gameFrame(gameFrame),
        m_pPlan(pPlan),
        m_data(data),
        m_activeGoalSet(activeGoalSet)
    {

    }

    unsigned GameFrame() const { return m_gameFrame; }
    IStrategizer::ConstOlcbpPlanContextRef Context() const { return m_context; }
    std::shared_ptr<IStrategizer::IOlcbpPlan> Plan() const { return m_pPlan; }

private:
    unsigned m_gameFrame;
    IStrategizer::OlcbpPlanNodeDataMap m_data;
    IStrategizer::IOlcbpPlan::NodeSet m_activeGoalSet;
    IStrategizer::OlcbpPlanContext m_context;
    std::shared_ptr<IStrategizer::IOlcbpPlan> m_pPlan;
};

class ClientMain : public QMainWindow, public IStrategizer::IMessagePumpObserver
{
    Q_OBJECT

public:
    enum ClientEvent
    {
        CLNTEVT_UiInit = (int)(QEvent::User) + 1,
        CLNTEVT_UiFinalize,
        CLNTEVT_PlanHistoryChanged
    };

    ClientMain(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    void OnMatchStart();
    void OnMatchEnd(bool p_isWinner);

protected:
    void showEvent(QShowEvent *p_pEvent);
    void closeEvent(QCloseEvent *p_pEvent);
    void timerEvent(QTimerEvent *pEvt);
    bool event(QEvent * pEvt);
    void OnUiInit();
    void OnUiFinalize();
    size_t GetProcessUsedMemoryKB();

private:
    void InitIMView();
    void InitStatsView();
    void InitPlannerView();
    void InitPlanHistoryView();
    void UpdateViews();
    void UpdateStatsView();
    void InitIdLookup();
    void NotifyMessegeSent(IStrategizer::Message* p_pMessage);
    void OnPlanHistoryChanged();
    void NotifyPlanHistoryChanged();
    Ui::ClientMainClass                ui;
    IStrategizer::IStrategizerEx    *m_pIStrategizer;
    IMViewWidget                    *m_pBldngDataIMWdgt;
    IMViewWidget                    *m_pGrndCtrlIMWdgt;
    std::vector<IMViewWidget*>        m_IMViews;
    bool                            m_isLearning;
    IStrategizer::GameTraceCollector *m_pTraceCollector;
    IStrategizer::CrossMap<unsigned, std::string>    m_idLookup;
    IStrategizer::PlanGraphView *m_pPlanGraphView;
    IStrategizer::PlanGraphView *m_pPlanHistoryView;
    std::vector<std::shared_ptr<PlanSnapshot>> m_planHistory;
    int m_updateTimerId;
    unsigned m_numGamesPlayed;
    size_t m_startMemoryUsage;
    bool m_isInGame;

    private slots:
    void OneHistorySliderValueChanged();
};

#endif // CLIENTMAIN_H

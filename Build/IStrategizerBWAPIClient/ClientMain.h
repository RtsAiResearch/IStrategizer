#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QMainWindow>
#include "ui_ClientMain.h"
#include "CrossMap.h"
#include "BwapiClient.h"
#include <Windows.h>
#include <vector>
#include "MessagePumpObserver.h"

namespace IStrategizer
{
    class RtsGame;
    class IStrategizerEx;
    class GameTraceCollector;
    class PlanGraphView;
}

class IMViewWidget;
class PlannerViewWidget;

class ClientMain : public QMainWindow, public BwapiClient, public IStrategizer::MessagePumpObserver
{
    Q_OBJECT

public:
    ClientMain(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~ClientMain();

protected:
    void showEvent(QShowEvent *p_pEvent);
    void closeEvent(QCloseEvent *p_pEvent);
    void OnClientUpdate();
    void OnClientLoopStart();
    void OnClientLoopEnd();
    void OnMatchStart();
    void OnMatchEnd(bool p_isWinner);
    void OnUnitCreate(BWAPI::Unit p_pUnit);
    void OnUnitDestroy(BWAPI::Unit p_pUnit);
    void OnUnitRenegade(BWAPI::Unit p_pUnit);
    void OnSendText(const std::string &p_text);
    void OnGameFrame();

private:
    void InitResourceManager();
    void InitIStrategizer();
    void InitIMView();
    void UpdateViews();
    void UpdateStatsView();
    void FinalizeIStrategizer();
    void InitPlannerView();
    void FinalizeViews();
    void InitIdLookup();
    void NotifyMessegeSent(IStrategizer::Message* p_pMessage);

    Ui::ClientMainClass ui;
    IStrategizer::IStrategizerEx *m_pIStrategizer;
    IStrategizer::RtsGame *m_pGameModel;
    IMViewWidget *m_pBldngDataIMWdgt;
    IMViewWidget *m_pGrndCtrlIMWdgt;
    std::vector<IMViewWidget*> m_IMViews;
    bool m_isLearning;
    IStrategizer::GameTraceCollector *m_pTraceCollector;
    IStrategizer::CrossMap<unsigned, std::string> m_idLookup;
    IStrategizer::PlanGraphView *m_pPlanGraphView;
    bool m_enemyPlayerUnitsCollected;
};

#endif // CLIENTMAIN_H

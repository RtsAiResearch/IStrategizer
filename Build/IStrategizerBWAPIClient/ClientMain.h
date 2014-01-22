#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QMainWindow>
#include <Windows.h>
#include <vector>
#include "ui_ClientMain.h"
#include "CrossMap.h"
#include "BwapiClient.h"
#include "MessagePumpObserver.h"
#include "RtsGame.h"

using namespace IStrategizer;
using namespace std;

namespace IStrategizer
{
    class RtsGame;
    class IStrategizerEx;
    class GameTraceCollector;
    class PlanGraphView;
}

class IMViewWidget;
class PlannerViewWidget;

class ClientMain : public QMainWindow, public BwapiClient, public MessagePumpObserver
{
    Q_OBJECT

public:
    ClientMain(RtsGame* p_RtsGame, QWidget *parent = 0, Qt::WindowFlags flags = 0);
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
    void OnUniRenegade(BWAPI::Unit p_pUnit);
    void OnSendText(const string &p_text);
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
    void NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_Message);

    Ui::ClientMainClass ui;
    IStrategizerEx *m_IStrategizer;
    RtsGame *m_GameModel;
    IMViewWidget *m_BuildingDataIMWidget;
    IMViewWidget *m_GroundControlIMWidget;
    vector<IMViewWidget*> m_IMViews;
    bool m_isLearning;
    GameTraceCollector *m_TraceCollector;
    CrossMap<unsigned, string> m_idLookup;
    PlanGraphView *m_PlanGraphView;
};

#endif // CLIENTMAIN_H

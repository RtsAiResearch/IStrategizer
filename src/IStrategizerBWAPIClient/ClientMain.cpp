#include "ClientMain.h"
#include "IMView.h"
#include "BWAPI.h"
#include "BWAPI\Client.h"
#include "IStrategizerEx.h"
#include "DataMessage.h"
#include "MessagePump.h"
#include "DefinitionCrossMapping.h"
#include "StarCraftGame.h"
#include "IMSystemManager.h"
#include "InfluenceMap.h"
#include "IMViewWidget.h"
#include "WorldClock.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "OnlinePlanExpansionExecution.h"
#include "GameTraceCollector.h"
#include "GraphScene.h"
#include "PlanGraphView.h"
#include "ObjectSerializer.h"
#include "WorldMap.h"
#include "GamePlayer.h"
#include "GameEntity.h"

#include <stdio.h>
#include <string>
#include <cassert>
#include <set>
#include <iostream>
#include <QGridLayout>
#include <QTableWidget>
#include <Psapi.h>
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

#define TilePositionFromUnitPosition(UnitPos)    (UnitPos / 32)
#define UnitPositionFromTilePosition(TilePos)    (TilePos * 32)

ClientMain::ClientMain(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    m_pIStrategizer(nullptr),
    m_pGameModel(nullptr),
    m_isLearning(false),
    m_pTraceCollector(nullptr),
    m_enemyPlayerUnitsCollected(false),
    m_pPlanGraphView(nullptr),
    m_pPlanHistoryView(nullptr),
    m_numGamesPlayed(0)
{
    ui.setupUi(this);
    IStrategizer::Init();
    g_MessagePump->RegisterForMessage(MSG_PlanStructureChange, this);

    connect(ui.sldHistoryFrame, SIGNAL(valueChanged(int)), SLOT(OneHistorySliderValueChanged()));
}
//////////////////////////////////////////////////////////////////////////
ClientMain::~ClientMain()
{
    FinalizeIStrategizer();

    delete m_pTraceCollector;
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitIStrategizer()
{
    IStrategizerParam param;

    try
    {
        m_pGameModel = new StarCraftGame;
        _ASSERTE(m_pGameModel);

        param.BuildingDataIMCellSize = TILE_SIZE;
        param.GrndCtrlIMCellSize = TILE_SIZE;
        param.OccupanceIMUpdateInterval = 250;
        param.GrndCtrlIMUpdateInterval = 1000;

        if (Broodwar->isReplay())
        {
            LogInfo("Learning from replay map: %s", Broodwar->mapFileName().c_str());

            Playerset players = Broodwar->getPlayers();
            for (auto player : players)
            {
                LogInfo("Replay Player[%d]: %s", player->getID(), player->getName().c_str());
            }

            param.Phase = PHASE_Offline;
            m_isLearning = true;
        }
        else
        {
            param.Phase = PHASE_Online;
            Broodwar->setLocalSpeed(0);
        }

        m_pIStrategizer = new IStrategizerEx(param, m_pGameModel);
        _ASSERTE(m_pIStrategizer);

        g_Database.Init();

        if (!m_pIStrategizer->Init())
        {
            LogError("Failed to initialize IStrategizer");
            return;
        }

        if (param.Phase == PHASE_Offline)
        {
            TID selfPlayerID = g_Database.PlayerMapping.GetBySecond(PLAYER_Self);
            LogInfo("Will learn demonstrations from self Player[%d]", );
            m_pTraceCollector = new GameTraceCollector(selfPlayerID);
        }

        // We postpone the IdLookup initialization until the engine is initialized and connected to the engine
        // and the engine Enums[*] table is fully initialized
        InitIdLookup();
    }
    catch (IStrategizer::Exception& e)
    {
        e.To(cout);
    }
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitIMView()
{
    if (m_IMViews.empty())
    {
        IMViewWidget    *pIMView = nullptr;
        QGridLayout        *gridLayout;

        // 1. Init Building Data IM
        pIMView = new IMViewWidget;
        m_pBldngDataIMWdgt= pIMView;
        m_IMViews.push_back(pIMView);

        gridLayout = new QGridLayout;
        ui.tbBuildingIM->setLayout(gridLayout);
        ui.tbBuildingIM->layout()->setAlignment(Qt::AlignCenter);
        ui.tbBuildingIM->layout()->addWidget(pIMView);
        ui.tbBuildingIM->layout()->setMargin(0);
        ui.tbBuildingIM->layout()->setSpacing(0);

        // 2. Init Ground Control IM
        pIMView = new IMViewWidget;
        m_pGrndCtrlIMWdgt = pIMView;
        m_IMViews.push_back(pIMView);

        gridLayout = new QGridLayout;
        ui.tbGrndCtrlIM->setLayout(gridLayout);
        ui.tbGrndCtrlIM->layout()->setAlignment(Qt::AlignCenter);
        ui.tbGrndCtrlIM->layout()->addWidget(pIMView);
        ui.tbGrndCtrlIM->layout()->setMargin(0);
        ui.tbGrndCtrlIM->layout()->setSpacing(0);
    }

    m_pBldngDataIMWdgt->SetIM(g_IMSysMgr.GetIM(IM_BuildingData));
    m_pGrndCtrlIMWdgt->SetIM(g_IMSysMgr.GetIM(IM_GroundControl));
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitPlannerView()
{
    if (m_pPlanGraphView == nullptr)
    {
        GraphScene *pGraphScene = new GraphScene(&m_idLookup);
        m_pPlanGraphView = new PlanGraphView(pGraphScene, &m_idLookup);
        ui.tbPlanner->layout()->addWidget(m_pPlanGraphView);
    }

    if (!m_isLearning)
    {
        auto pPlanner = m_pIStrategizer->Planner()->ExpansionExecution();
        m_pPlanGraphView->View(pPlanner->Plan(), &pPlanner->GetContext());
    }
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitPlanHistoryView()
{
    if (m_pPlanHistoryView == nullptr)
    {
        GraphScene *pGraphScene = new GraphScene(&m_idLookup);
        m_pPlanHistoryView = new PlanGraphView(pGraphScene, &m_idLookup);
        ui.tbPlanHistory->layout()->addWidget(m_pPlanHistoryView);
    }

    m_planHistory.clear();
    m_pPlanHistoryView->View(nullptr, nullptr);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitIdLookup()
{
    for (unsigned currID = 0; currID < ENUMS_SIZE; ++currID)
    {
        if (Enums[currID] != nullptr)
        {
            string enumStr(Enums[currID]);
            m_idLookup.SetByFirst(currID, enumStr);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::FinalizeIStrategizer()
{
    SAFE_DELETE(m_pIStrategizer);

	RtsGame::FinalizeStaticData();
    SAFE_DELETE(m_pGameModel);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::showEvent(QShowEvent *pEvent)
{
    if (!ClientInitialized())
    {
        InitClient();
    }

    QMainWindow::showEvent(pEvent);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::closeEvent(QCloseEvent *pEvent) 
{
    ShutdownClient();
    QMainWindow::closeEvent(pEvent);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnClientLoopStart()
{
    QApplication::postEvent(this, new QEvent((QEvent::Type)CLNTEVT_UiFinalize));
    // Give the app time to finalize itself before finalizing the engine
    // FIXME: Replace the sleep with synchronization event for robustness
    Sleep(2000);
    FinalizeIStrategizer();

    // Dump memory leaks if exist
    EngineObject::DumpAliveObjects();
    // Make sure that we start from a clean slate

    // Hard Reset is always a bad idea and can cause inconsistency
	// EngineObject::FreeMemoryPool();

    m_enemyPlayerUnitsCollected = false;
    InitIStrategizer();
    QApplication::postEvent(this, new QEvent((QEvent::Type)CLNTEVT_UiInit));
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnClientLoopEnd()
{
    ++m_numGamesPlayed;
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnSendText(const string& p_text)
{
    Broodwar->printf("[IStrategizer] You typed '%s'!",p_text.c_str());
    TextMessage *pTxtMsg;

    pTxtMsg = new TextMessage(Broodwar->getFrameCount(), MSG_Input, new string(p_text));
    g_MessagePump->Send(pTxtMsg);

    if (p_text == "export-statics")
        m_pGameModel->ExportStaticData();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUnitCreate(BWAPI::Unit p_pUnit)
{
    EntityMessageData    *pData = nullptr;
    EntityCreateMessage    *pMsg = nullptr;

    pData = new EntityMessageData;
    _ASSERTE(pData);

    _ASSERTE(p_pUnit);
    pData->EntityId = p_pUnit->getID();
    pData->OwnerId = g_Database.PlayerMapping.GetByFirst(p_pUnit->getPlayer()->getID());
    pData->EntityType = g_Database.EntityMapping.GetByFirst(p_pUnit->getType());

    if (p_pUnit->getType().isBuilding())
    {
        pData->X = UnitPositionFromTilePosition(p_pUnit->getTilePosition().x);
        pData->Y = UnitPositionFromTilePosition(p_pUnit->getTilePosition().y);
    }
    else
    {
        pData->X = p_pUnit->getLeft();
        pData->Y = p_pUnit->getTop();
    }

    pMsg = new EntityCreateMessage(Broodwar->getFrameCount(), MSG_EntityCreate, pData);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUnitDestroy(BWAPI::Unit p_pUnit)
{
    EntityMessageData *pData = nullptr;
    EntityDestroyMessage    *pMsg = nullptr;

    pData = new EntityMessageData;
    _ASSERTE(pData);

    _ASSERTE(p_pUnit);

    pData->EntityId = p_pUnit->getID();
    pData->OwnerId = g_Database.PlayerMapping.GetByFirst(p_pUnit->getPlayer()->getID());

    if (p_pUnit->getType().isBuilding())
    {
        pData->X = UnitPositionFromTilePosition(p_pUnit->getTilePosition().x);
        pData->Y = UnitPositionFromTilePosition(p_pUnit->getTilePosition().y);
    }
    else
    {
        pData->X = p_pUnit->getLeft();
        pData->Y = p_pUnit->getTop();
    }

    pMsg = new EntityDestroyMessage(Broodwar->getFrameCount(), MSG_EntityDestroy, pData);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUnitRenegade(BWAPI::Unit p_pUnit)
{
    EntityMessageData *pData = nullptr;
    EntityCreateMessage *pMsg = nullptr;

    pData = new EntityMessageData;
    _ASSERTE(pData);

    _ASSERTE(p_pUnit);
    pData->EntityId = p_pUnit->getID();
    pData->OwnerId = g_Database.PlayerMapping.GetByFirst(p_pUnit->getPlayer()->getID());
    pData->EntityType = g_Database.EntityMapping.GetByFirst(p_pUnit->getType());

    if (p_pUnit->getType().isBuilding())
    {
        pData->X = UnitPositionFromTilePosition(p_pUnit->getTilePosition().x);
        pData->Y = UnitPositionFromTilePosition(p_pUnit->getTilePosition().y);
    }
    else
    {
        pData->X = p_pUnit->getLeft();
        pData->Y = p_pUnit->getTop();
    }

    pMsg = new EntityCreateMessage(Broodwar->getFrameCount(), MSG_EntityRenegade, pData);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnMatchStart()
{
    Message *pMsg;

    pMsg = new Message(Broodwar->getFrameCount(), MSG_GameStart);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnMatchEnd(bool p_isWinner)
{
    GameEndMessageData    *pData = nullptr;
    GameEndMessage        *pMsg = nullptr;

    pData = new GameEndMessageData;
    _ASSERTE(pData);

    Player player = Broodwar->getPlayer(g_Database.PlayerMapping.GetBySecond(PLAYER_Self));
    pData->IsWinner = p_isWinner;
    pData->Score = player->getBuildingScore() + player->getRazingScore() + player->getUnitScore() + player->getCustomScore() + player->getKillScore();
    pData->MapName = Broodwar->mapFileName();
    pData->EnemyRace = g_Game->GetRace(Broodwar->getPlayer(g_Database.PlayerMapping.GetBySecond(PLAYER_Enemy))->getRace().getID())->ToString();
    pMsg = new GameEndMessage(Broodwar->getFrameCount(), MSG_GameEnd, pData);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::UpdateStatsView()
{
    ui.lblGameCyclesData->setText(tr("%1").arg(m_pIStrategizer->Clock().ElapsedGameCycles()));

    EntityList workers;
    map<ObjectStateType, set<TID>> workersState;

    m_pGameModel->Self()->Entities(m_pGameModel->Self()->Race()->GetWorkerType(), workers);

    ui.lblWorkersCountData->setText(QString("%1").arg(workers.size()));

    for (auto workerId : workers)
    {
        GameEntity* worker = m_pGameModel->Self()->GetEntity(workerId);
        workersState[(ObjectStateType)worker->Attr(EOATTR_State)].insert(worker->Id());
    }

    QTableWidgetItem* cell = NULL;
    for(int row = 0; row < ui.tblWorkerState->rowCount(); ++row)
    {
        cell = ui.tblWorkerState->item(row, 1);
        ObjectStateType state = (ObjectStateType)cell->data(Qt::UserRole).toInt();

        QString txt =QString("[%1]{").arg(workersState[state].size());
        for (auto workerId : workersState[state])
        {
            auto pEntity = m_pGameModel->Self()->GetEntity(workerId);
            if (pEntity != nullptr)
            {
                bool isLocked = pEntity->IsLocked();
                txt += QString("%1").arg(workerId);
                txt += QString("[%2],").arg(isLocked ? "L" : "F");
            }
        }

        txt += "}";

        cell->setText(txt);
    }

    ui.tblWorkerState->resizeColumnsToContents();

    ui.lblFPSData->setText(tr("%1").arg(Broodwar->getFPS()));

    int currMemoryUsage = (int)GetProcessUsedMemoryKB();
    ui.lblCurrUsedMemoryData->setText(QString("%1").arg(currMemoryUsage));

    int deltaMemoryUsage = abs((int)currMemoryUsage - (int)m_startMemoryUsage);
    ui.lblDeltaMemoryData->setText(QString("%1").arg(deltaMemoryUsage));

    ui.lblObjectsMemoryData->setText(QString("%1").arg(EngineObject::AliveObjectsMemoryUsage() / 1024));
    ui.lblObjectsCountData->setText(QString("%1").arg(EngineObject::AliveObjectsCount()));

}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnClientUpdate()
{
    if (!Broodwar->isReplay() &&
        !m_enemyPlayerUnitsCollected)
    {
        // This to solve the bug that the game does not send  messages about creating enemy units at game start
        TID enemyPlayerID = g_Database.PlayerMapping.GetBySecond(PLAYER_Enemy);
        const Unitset &enemyUnits = Broodwar->getPlayer(enemyPlayerID)->getUnits();

        for (Unitset::iterator itr = enemyUnits.begin();
            itr != enemyUnits.end(); ++itr)
        {
            OnUnitCreate(*itr);
        }

        m_enemyPlayerUnitsCollected = !enemyUnits.empty();
    }

    try
    {
        m_pIStrategizer->Update(Broodwar->getFrameCount());

        /*if (Broodwar->getFrameCount() % 10 == 0)
        {
        RtsGame* pSnapshot = g_Game->Snapshot();
        m_snapshots[Broodwar->getFrameCount()].first = pSnapshot;

        g_ObjectSerializer.Serialize(pSnapshot, "rts.bin");

        StarCraftGame* pCopy = new StarCraftGame();
        g_ObjectSerializer.Deserialize(pCopy, "rts.bin");
        pCopy->Init();
        pCopy->Map()->Update();
        m_snapshots[Broodwar->getFrameCount()].second = pCopy;
        }*/
    }
    catch (IStrategizer::Exception &e)
    {
        e.To(cout);
    }


}
//////////////////////////////////////////////////////////////////////////
void ClientMain::UpdateViews()
{
    for (unsigned i = 0, size = m_IMViews.size(); i < size; ++i)
        m_IMViews[i]->update();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnGameFrame()
{
    if (Broodwar->isReplay())
    {
        _ASSERTE(m_pTraceCollector);
        m_pTraceCollector->OnGameFrame();
    }
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::NotifyMessegeSent(Message* p_pMessage)
{
    _ASSERTE(p_pMessage != nullptr);

    if (p_pMessage->MessageTypeID() == MSG_PlanStructureChange && m_pPlanGraphView != nullptr )
    {
        Message* pPlanChangeMsg = static_cast<Message*>(p_pMessage);

        auto pPlanner = m_pIStrategizer->Planner()->ExpansionExecution();

        if (m_pPlanGraphView != nullptr)
            m_pPlanGraphView->NotifyGraphStructureChange(pPlanner->Plan());

        if (!m_isLearning && m_pPlanHistoryView != nullptr)
        {
            pPlanner->Plan()->Lock();

            shared_ptr<PlanSnapshot> pSnapshot(new PlanSnapshot(pPlanChangeMsg->GameCycle(),
                shared_ptr<IOlcbpPlan>(pPlanner->Plan()->Clone()),
                pPlanner->GetContext().Data,
                pPlanner->GetContext().ActiveGoalSet));

            pPlanner->Plan()->Unlock();

            m_planHistory.push_back(pSnapshot);
            NotifyPlanHistoryChanged();

            m_pPlanHistoryView->NotifyGraphStructureChange(pPlanner->Plan());
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitStatsView()
{
    if (ui.tblWorkerState->rowCount() == 0)
    {
        ui.tblWorkerState->clear();

        ui.tblWorkerState->setColumnCount(2);
        ui.tblWorkerState->setRowCount(COUNT(ObjectStateType));
        ui.tblWorkerState->horizontalHeader()->hide();
        ui.tblWorkerState->verticalHeader()->hide();

        int row = 0;

        for(auto workerState = START(ObjectStateType);
            workerState != END(ObjectStateType);
            workerState = (ObjectStateType)((int)workerState + 1))
        {
            QTableWidgetItem* cell = new QTableWidgetItem(QString::fromLocal8Bit(Enums[(int)workerState]));
            cell->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            QVariant objStateType((int)workerState);
            ui.tblWorkerState->setItem(row, 0, cell);

            cell = new QTableWidgetItem("-");
            cell->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            cell->setData(Qt::UserRole, objStateType);
            ui.tblWorkerState->setItem(row, 1, cell);

            ui.tblWorkerState->setRowHeight(row, 20);
            ++row;
        }

        ui.tblWorkerState->resizeColumnsToContents();
    }

    m_startMemoryUsage = GetProcessUsedMemoryKB();
    ui.lblStartMemoryData->setText(QString("%1").arg(m_startMemoryUsage));
    ui.lblGamesPlayedData->setText(QString("%1").arg(m_numGamesPlayed));
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::timerEvent(QTimerEvent *pEvt)
{
    if (BroodwarPtr && BroodwarPtr->isInGame())
    {
        UpdateViews();
        UpdateStatsView();
    }
}
//////////////////////////////////////////////////////////////////////////
bool ClientMain::event(QEvent * pEvt)
{
    _ASSERTE(pEvt);
    ClientEvent evt = (ClientEvent)pEvt->type();

    if (evt == CLNTEVT_UiInit)
    {

        OnUiInit();
        return true;
    }
    else if (evt == CLNTEVT_UiFinalize)
    {
        OnUiFinalize();
        return true;
    }
    else if (evt == CLNTEVT_PlanHistoryChanged)
    {
        OnPlanHistoryChanged();
        return true;
    }
    else
    {
        return QMainWindow::event(pEvt);
    }
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUiInit()
{
    InitIMView();
    InitPlannerView();
    InitPlanHistoryView();
    InitStatsView();
    m_updateTimerId = startTimer(1000);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUiFinalize()
{
    killTimer(m_updateTimerId);

    // Give IM widgets a chance to clear its buffer and redraw
    UpdateViews();

    for (unsigned i = 0, size = m_IMViews.size(); i < size; ++i)
        m_IMViews[i]->SetIM(nullptr);

    if (m_pPlanGraphView != nullptr)
        m_pPlanGraphView->View(nullptr, nullptr);

    if (m_pPlanHistoryView != nullptr)
        m_pPlanHistoryView->View(nullptr, nullptr);

    for (auto& snapshot : m_planHistory)
    {
        auto nodes = snapshot->Plan()->GetNodes();
        for (auto nodeId : nodes)
        {
            auto pNode = snapshot->Plan()->GetNode(nodeId);
            snapshot->Plan()->RemoveNode(nodeId);
            delete pNode;
        }
    }
    m_planHistory.clear();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::NotifyPlanHistoryChanged()
{
    QApplication::postEvent(this, new QEvent((QEvent::Type)CLNTEVT_PlanHistoryChanged));
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnPlanHistoryChanged()
{
    _ASSERTE(m_pPlanHistoryView);

    ui.sldHistoryFrame->setMaximum(m_planHistory.size() - 1);
    ui.sldHistoryFrame->setValue(m_planHistory.size() - 1);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OneHistorySliderValueChanged()
{
    if (!m_planHistory.empty())
    {
        auto idx = ui.sldHistoryFrame->value();
        auto history = m_planHistory[idx];
        m_pPlanHistoryView->View(&*history->Plan(), &history->Context());
    }
}
//////////////////////////////////////////////////////////////////////////
size_t ClientMain::GetProcessUsedMemoryKB()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;

    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

    return pmc.PrivateUsage / 1024;
}

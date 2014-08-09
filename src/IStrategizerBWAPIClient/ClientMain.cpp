#include "ClientMain.h"
#include "IMView.h"
#include "IStrategizerEx.h"
#include "DataMessage.h"
#include "MessagePump.h"
#include "IMSystemManager.h"
#include "InfluenceMap.h"
#include "IMViewWidget.h"
#include "WorldClock.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "OnlinePlanExpansionExecution.h"
#include "GraphScene.h"
#include "PlanGraphView.h"
#include "ObjectSerializer.h"
#include "WorldMap.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "AIModuleLoader.h"

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
using namespace std;

ClientMain* g_pClientInst = nullptr;

void ModueOnMatchStart()
{
    g_pClientInst->OnMatchStart();
}

void ModuleOnMatchEnd(bool isWinner)
{
    g_pClientInst->OnMatchEnd(isWinner);
}

ClientMain::ClientMain(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    m_pIStrategizer(nullptr),
    m_pGameModel(nullptr),
    m_isLearning(false),
    m_pTraceCollector(nullptr),
    m_pPlanGraphView(nullptr),
    m_pPlanHistoryView(nullptr),
    m_numGamesPlayed(0)
{
    g_pClientInst = this;
    ui.setupUi(this);
    IStrategizer::Init();

    connect(ui.sldHistoryFrame, SIGNAL(valueChanged(int)), SLOT(OneHistorySliderValueChanged()));
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

    m_pBldngDataIMWdgt->SetIM(m_pIStrategizer->IMSysMgr()->GetIM(IM_BuildingData));
    m_pGrndCtrlIMWdgt->SetIM(m_pIStrategizer->IMSysMgr()->GetIM(IM_GroundControl));
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
void ClientMain::showEvent(QShowEvent *pEvent)
{
    if (!AiModuleLoaderInitialized())
    {
        AiModuleLoaderSetMatchStartCallback(&ModueOnMatchStart);
        AiModuleLoaderSetMatchEndCallback(&ModuleOnMatchEnd);
        AiModuleLoaderInit("Yarmouk.dll");
    }

    QMainWindow::showEvent(pEvent);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::closeEvent(QCloseEvent *pEvent) 
{
    AiModuleLoaderShutdown();
    QMainWindow::closeEvent(pEvent);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnMatchStart()
{
    m_pIStrategizer = (IStrategizerEx*)AIModuleLoaderGetEngine();
    m_pGameModel = m_pIStrategizer->GameModel();
    InitIdLookup();
    m_pIStrategizer->RegisterForMessage(MSG_PlanStructureChange, this);

    QApplication::postEvent(this, new QEvent((QEvent::Type)CLNTEVT_UiInit));
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnMatchEnd(bool p_isWinner)
{
    ++m_numGamesPlayed;

    QApplication::postEvent(this, new QEvent((QEvent::Type)CLNTEVT_UiFinalize));
    // Give the app time to finalize itself before finalizing the engine
    // FIXME: Replace the sleep with synchronization event for robustness
    Sleep(2000);

    // Dump memory leaks if exist
    EngineObject::DumpAliveObjects();
    // Make sure that we start from a clean slate

    // Hard Reset is always a bad idea and can cause inconsistency
    // EngineObject::FreeMemoryPool();

    QApplication::postEvent(this, new QEvent((QEvent::Type)CLNTEVT_UiInit));
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::timerEvent(QTimerEvent *pEvt)
{
    if (m_pIStrategizer && 
        m_pIStrategizer->GameModelImpl() &&
        m_pIStrategizer->GameModelImpl()->IsInGame())
    {
        UpdateViews();
        UpdateStatsView();
    }
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::UpdateStatsView()
{
    ui.lblGameCyclesData->setText(tr("%1").arg(m_pGameModel->Clock().ElapsedGameCycles()));

    map<ObjectStateType, set<TID>> workersState;
    auto workerType = m_pGameModel->Self()->Race()->GetWorkerType();

    int workerCount = 0;
    for (auto& entityR : m_pGameModel->Self()->Entities())
    {
        if (entityR.second->TypeId() == workerType)
        {
            workersState[(ObjectStateType)entityR.second->P(OP_State)].insert(entityR.first);
            ++workerCount;
        }
    }

    ui.lblWorkersCountData->setText(QString("%1").arg(workerCount));

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

    ui.lblFPSData->setText(tr("%1").arg("-"));

    int currMemoryUsage = (int)GetProcessUsedMemoryKB();
    ui.lblCurrUsedMemoryData->setText(QString("%1").arg(currMemoryUsage));

    int deltaMemoryUsage = abs((int)currMemoryUsage - (int)m_startMemoryUsage);
    ui.lblDeltaMemoryData->setText(QString("%1").arg(deltaMemoryUsage));

    ui.lblObjectsMemoryData->setText(QString("%1").arg(EngineObject::AliveObjectsMemoryUsage() / 1024));
    ui.lblObjectsCountData->setText(QString("%1").arg(EngineObject::AliveObjectsCount()));

}
//////////////////////////////////////////////////////////////////////////
void ClientMain::UpdateViews()
{
    for (unsigned i = 0, size = m_IMViews.size(); i < size; ++i)
        m_IMViews[i]->update();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::NotifyMessegeSent(Message* p_pMessage)
{
    _ASSERTE(p_pMessage != nullptr);

    if (p_pMessage->TypeId() == MSG_PlanStructureChange && m_pPlanGraphView != nullptr )
    {
        Message* pPlanChangeMsg = static_cast<Message*>(p_pMessage);

        auto pPlanner = m_pIStrategizer->Planner()->ExpansionExecution();

        if (m_pPlanGraphView != nullptr)
            m_pPlanGraphView->NotifyGraphStructureChange(pPlanner->Plan());

        if (!m_isLearning && m_pPlanHistoryView != nullptr)
        {
            pPlanner->Plan()->Lock();

            shared_ptr<PlanSnapshot> pSnapshot(new PlanSnapshot(pPlanChangeMsg->GameFrame(),
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
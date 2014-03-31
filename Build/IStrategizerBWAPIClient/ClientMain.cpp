#include "ClientMain.h"
#include <stdio.h>
#include <Windows.h>
#include <string>
#include <cassert>
#include <set>
#include "IMView.h"
#include "BWAPI.h"
#include "BWAPI\Client.h"
#include "CmnHdr.h"
#include "IStrategizerEx.h"
#include "DataMessage.h"
#include "MessagePump.h"
#include "DefinitionCrossMapping.h"
#include "StarCraftGame.h"
#include "IMSystemManager.h"
#include "InfluenceMap.h"
#include "IMViewWidget.h"
#include <QGridLayout>
#include "WorldClock.h"
#include <iostream>
#include "OnlineCaseBasedPlannerEx.h"
#include "OnlinePlanExpansionExecution.h"
#include "GameTraceCollector.h"
#include "GraphScene.h"
#include "PlanGraphView.h"

using namespace IStrategizer;
using namespace StarCraftModel;
using namespace BWAPI;
using namespace std;

#define TilePositionFromUnitPosition(UnitPos)    (UnitPos / 32)
#define UnitPositionFromTilePosition(TilePos)    (TilePos * 32)

ClientMain::ClientMain(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    m_pIStrategizer(nullptr),
    m_pGameModel(nullptr),
    m_isLearning(false),
    m_pTraceCollector(nullptr)
{
    ui.setupUi(this);
    IStrategizer::Init();
    g_MessagePump.RegisterForMessage(MSG_PlanStructureChange, this);
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
        assert(m_pGameModel);

        param.BuildingDataIMCellSize = TILE_SIZE;
        param.GrndCtrlIMCellSize = TILE_SIZE;
        param.OccupanceIMUpdateInterval = 250;
        param.GrndCtrlIMUpdateInterval = 1000;

        if (Broodwar->isReplay())
        {
            Playerset players = Broodwar->getPlayers();
            TID playerToObserveID = g_Database.PlayerMapping.GetBySecond(PLAYER_Self);

            m_pTraceCollector = new GameTraceCollector(playerToObserveID);
            param.Phase = PHASE_Offline;
            m_isLearning = true;
        }
        else
            param.Phase = PHASE_Online;

        m_pIStrategizer = new IStrategizerEx(param, m_pGameModel);
        assert(m_pIStrategizer);
    }
    catch (IStrategizer::Exception& e)
    {
        e.To(cout);
    }

    m_pBldngDataIMWdgt->SetIM(g_IMSysMgr.GetIM(IM_BuildingData));
    m_pGrndCtrlIMWdgt->SetIM(g_IMSysMgr.GetIM(IM_GroundControl));

    // We postpone the IdLookup initialization until the engine is initialized and connected to the engine
    // and the engine Enums[*] table is fully initialized
    InitIdLookup();
    m_pPlanGraphView->View(m_pIStrategizer->Planner()->ExpansionExecution()->Plan());
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitIMView()
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
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitPlannerView()
{
    GraphScene *pGraphScene = new GraphScene(&m_idLookup);
    m_pPlanGraphView = new PlanGraphView(pGraphScene, &m_idLookup);
    ui.plannerGridLayout->addWidget(m_pPlanGraphView);
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
    delete m_pIStrategizer;
    m_pIStrategizer = nullptr;

    delete m_pGameModel;
    m_pGameModel = nullptr;
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::FinalizeViews()
{
    for (size_t i = 0, size = m_IMViews.size(); i < size; ++i)
        m_IMViews[i]->SetIM(nullptr);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::showEvent(QShowEvent *pEvent)
{
    if (!ClientInitialized())
    {
        InitClient();
        InitIMView();
        InitPlannerView();
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
    if (!Broodwar->isReplay())
    {
        InitResourceManager();
    }

    InitIStrategizer();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnClientLoopEnd()
{
    FinalizeIStrategizer();
    // Give IM widgets a chance to clear its buffer and redraw
    UpdateViews();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnSendText(const string& p_text)
{
    Broodwar->printf("[IStrategizer]You typed '%s'!",p_text.c_str());
    TextMessage *pTxtMsg;

    pTxtMsg = new TextMessage(Broodwar->getFrameCount(), MSG_Input, new string(p_text));
    g_MessagePump.Send(pTxtMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUnitCreate(BWAPI::Unit p_pUnit)
{
    EntityMessageData    *pData = nullptr;
    EntityCreateMessage    *pMsg = nullptr;

    pData = new EntityMessageData;
    assert(pData);

    assert(p_pUnit);
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

    pMsg = new EntityCreateMessage(Broodwar->getFrameCount(), MSG_EntityCreate, pData);
    assert(pMsg);

    g_MessagePump.Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUnitDestroy(BWAPI::Unit p_pUnit)
{
    EntityMessageData        *pData = nullptr;
    EntityDestroyMessage    *pMsg = nullptr;

    pData = new EntityMessageData;
    assert(pData);

    assert(p_pUnit);

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
    assert(pMsg);

    g_MessagePump.Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUniRenegade(BWAPI::Unit p_pUnit)
{
    EntityMessageData        *pData = nullptr;
    EntityRenegadeMessage    *pMsg = nullptr;

    pData = new EntityMessageData;
    assert(pData);

    assert(p_pUnit);
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

    pMsg = new EntityRenegadeMessage(Broodwar->getFrameCount(), MSG_EntityRenegade, pData);
    assert(pMsg);

    g_MessagePump.Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnMatchStart()
{
    Message *pMsg;

    pMsg = new Message(Broodwar->getFrameCount(), MSG_GameStart);
    assert(pMsg);

    g_MessagePump.Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnMatchEnd(bool p_isWinner)
{
    GameEndMessageData    *pData = nullptr;
    GameEndMessage        *pMsg = nullptr;

    pData = new GameEndMessageData;
    assert(pData);

    pData->IsWinner = p_isWinner;

    pMsg = new GameEndMessage(Broodwar->getFrameCount(), MSG_GameEnd, pData);
    assert(pMsg);

    g_MessagePump.Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::UpdateStatsView()
{
    float engineRatio, gameRatio;

    ui.lblEngineCycleData->setText(tr("%1").arg(m_pIStrategizer->Clock().ElapsedEngineCycles()));
    ui.lblGameCyclesData->setText(tr("%1").arg(Broodwar->getFrameCount()));

    engineRatio = (float)m_pIStrategizer->Clock().ElapsedEngineCycles();
    gameRatio = (float)Broodwar->getFrameCount();

    if (engineRatio > gameRatio)
    {
        engineRatio /= gameRatio;
        gameRatio = 1.0;
    }
    else
    {
        gameRatio /= engineRatio;
        engineRatio = 1.0;
    }

    ui.lblEngineRatioData->setText(tr("%1").arg(engineRatio));
    ui.lblGameRatioData->setText(tr("%1").arg(gameRatio));
    ui.lblFrameDiffData->setText(tr("%1").arg(
        abs((int)(m_pIStrategizer->Clock().ElapsedEngineCycles() - Broodwar->getFrameCount()))));

}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnClientUpdate()
{
    static bool enemyPlayerCollected = false;

    if (!Broodwar->isReplay() &&
        !enemyPlayerCollected)
    {
        // This to solve the bug that the game does not send  messages about creating enemy units at game start
        TID enemyPlayerID = g_Database.PlayerMapping.GetBySecond(PLAYER_Enemy);
        const Unitset &enemyUnits = Broodwar->getPlayer(enemyPlayerID)->getUnits();

        for (Unitset::iterator itr = enemyUnits.begin();
            itr != enemyUnits.end(); ++itr)
        {
            OnUnitCreate(*itr);
        }

        enemyPlayerCollected = !enemyUnits.empty();
    }

    try
    {
        m_pIStrategizer->Update(Broodwar->getFrameCount());
    }
    catch (IStrategizer::Exception &e)
    {
        e.To(cout);
    }

    UpdateViews();
    UpdateStatsView();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::UpdateViews()
{
    for (size_t i = 0, size = m_IMViews.size(); i < size; ++i)
        m_IMViews[i]->update();

    // m_pPlanGraphView->update();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitResourceManager()
{
	//send each worker to the mineral field that is closest to it
	for(Unitset::iterator i = Broodwar->self()->getUnits().begin(); i != Broodwar->self()->getUnits().end(); i++)
	{
		if ((*i)->getType().isWorker())
		{
			Unit closestMineral=nullptr;
			for (Unitset::iterator m = Broodwar->getMinerals().begin(); m!=Broodwar->getMinerals().end(); m++)
			{
				if (closestMineral==nullptr || (*i)->getDistance(*m) < (*i)->getDistance(closestMineral))
				{
					closestMineral = *m;
					
					// [Debugging]
					auto type = closestMineral->getType();
					auto initialType = closestMineral->getInitialType();
					auto nResources = closestMineral->getInitialResources();
				}
			}
			if (closestMineral!=nullptr)
				(*i)->rightClick(closestMineral);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnGameFrame()
{
    if (Broodwar->isReplay())
    {
        assert(m_pTraceCollector);
        m_pTraceCollector->OnGameFrame();
    }
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::NotifyMessegeSent(Message* p_pMessage)
{
    assert(p_pMessage != nullptr);

    if (p_pMessage->MessageTypeID() == MSG_PlanStructureChange)
    {
        m_pPlanGraphView->OnPlanStructureChange();
    }
}

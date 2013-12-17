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
#include "PlannerViewWidget.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "OnlinePlanExpansionExecutionEx.h"

using namespace IStrategizer;
using namespace StarCraftModel;
using namespace BWAPI;
using namespace std;

#define TilePositionFromUnitPosition(UnitPos)	(UnitPos / 32)
#define UnitPositionFromTilePosition(TilePos)	(TilePos * 32)

ClientMain::ClientMain(QWidget *parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags), m_pIStrategizer(NULL), m_pGameModel(NULL), m_isLearning(true)
{
	ui.setupUi(this);
}
//////////////////////////////////////////////////////////////////////////
ClientMain::~ClientMain()
{
	FinalizeIStrategizer();
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
		param.GroundControlIMCellSize = TILE_SIZE;
		param.IMSysUpdateInterval = 64;

		m_pIStrategizer = new IStrategizerEx(param, PHASE_Online, m_pGameModel);
		assert(m_pIStrategizer);
	}
	catch (IStrategizer::Exception& e)
	{
		e.To(cout);
	}

	m_pBldngDataIMWdgt->SetIM(g_IMSysMgr.GetIM(IM_BuildingData));
	m_pGrndCtrlIMWdgt->SetIM(g_IMSysMgr.GetIM(IM_GroundControl));
	m_pPlannerViewWdgt->Planner(m_pIStrategizer->Planner()->ExpansionExecution());
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitIMView()
{
	IMViewWidget	*pIMView = NULL;
	QGridLayout		*gridLayout;

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
	m_pPlannerViewWdgt = new PlannerViewWidget;

	QGridLayout		*gridLayout;

	gridLayout = new QGridLayout;
	ui.tbPlanner->setLayout(gridLayout);
	ui.tbPlanner->layout()->setAlignment(Qt::AlignCenter);
	ui.tbPlanner->layout()->addWidget(m_pPlannerViewWdgt);
	ui.tbPlanner->layout()->setMargin(0);
	ui.tbPlanner->layout()->setSpacing(0);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::FinalizeIStrategizer()
{
	delete m_pIStrategizer;
	m_pIStrategizer = NULL;

	delete m_pGameModel;
	m_pGameModel = NULL;
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
	EntityMessageData	*pData = NULL;
	EntityCreateMessage	*pMsg = NULL;

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
	EntityMessageData		*pData = NULL;
	EntityDestroyMessage	*pMsg = NULL;

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
	EntityMessageData		*pData = NULL;
	EntityRenegadeMessage	*pMsg = NULL;

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
	GameEndMessageData	*pData = NULL;
	GameEndMessage		*pMsg = NULL;

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

	ui.lblEngineCycleData->setText(tr("%1").arg(g_WorldClock.ElapsedEngineCycles()));
	ui.lblGameCyclesData->setText(tr("%1").arg(Broodwar->getFrameCount()));

	engineRatio = (float)g_WorldClock.ElapsedEngineCycles();
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
		abs((int)(g_WorldClock.ElapsedEngineCycles() - Broodwar->getFrameCount()))));

}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnClientUpdate()
{
	static bool enemyPlayerCollected = false;

	if (!enemyPlayerCollected)
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
	for (int i = 0, size = m_IMViews.size(); i < size; ++i)
		m_IMViews[i]->update();

	m_pPlannerViewWdgt->update();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitResourceManager()
{
	//send each worker to the mineral field that is closest to it
	for(Unitset::iterator i = Broodwar->self()->getUnits().begin(); i != Broodwar->self()->getUnits().end(); i++)
	{
		if ((*i)->getType().isWorker())
		{
			Unit closestMineral=NULL;
			for (Unitset::iterator m = Broodwar->getMinerals().begin(); m!=Broodwar->getMinerals().end(); m++)
			{
				if (closestMineral==NULL || (*i)->getDistance(*m) < (*i)->getDistance(closestMineral))
					closestMineral = *m;
			}
			if (closestMineral!=NULL)
				(*i)->rightClick(closestMineral);
		}
	}
}
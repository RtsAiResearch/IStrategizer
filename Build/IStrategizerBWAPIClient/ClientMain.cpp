#include "ClientMain.h"
#include <stdio.h>
#include <Windows.h>
#include <string>
#include <cassert>
#include <set>
#include "IMView.h"
#include "BWAPI.h"
#include "BWAPI\Client.h"
#include "BWTA.h"
#include "CmnHdr.h"
#include "IStrategizerEx.h"
#include "DataMessage.h"
#include "MessagePump.h"
#include "DefinitionCrossMapping.h"
#include "StarCraftGame.h"
#include "IMSystemManager.h"
#include "InfluenceMap.h"
#include "IMViewWidget.h"
#include "QtGui/QGridLayout"

using namespace IStrategizer;
using namespace StarCraftModel;
using namespace BWAPI;
using namespace std;

void drawStats();
void drawBullets();
void drawVisibilityData();
void drawTerrainData();
void showPlayers();
void showForces();
bool show_bullets;
bool show_visibility_data;

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();
volatile bool g_bShutdown = false;

#define TilePositionFromUnitPosition(UnitPos)	(UnitPos / 32)
#define UnitPositionFromTilePosition(TilePos)	(TilePos * 32)

ClientMain::ClientMain(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), m_hBwapiThread(NULL), m_bClientInitialized(false),
	m_pIStrategizer(NULL), m_pGameModel(NULL)
{
	ui.setupUi(this);
	
}
//////////////////////////////////////////////////////////////////////////
ClientMain::~ClientMain()
{
	FinalizeIStrategizer();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitClient()
{
	m_hBwapiThread = chBEGINTHREADEX(NULL, 0, BwapiThreadStart, this, 0, NULL);
	assert(m_hBwapiThread);

	if (m_hBwapiThread)
		m_bClientInitialized = true;

	InitIMView();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitIStrategizer()
{
	IStrategizerParam param;

	m_pGameModel = new StarCraftGame;
	assert(m_pGameModel);

	param.BuildingDataIMCellSize = TILE_SIZE;
	param.GroundControlIMCellSize = TILE_SIZE;
	param.IMSysUpdateInterval = 64;

	m_pIStrategizer = new IStrategizerEx(param, PHASE_Online, m_pGameModel);
	assert(m_pIStrategizer);

	m_pBuildingDataIMWidget->SetIM(g_IMSysMgr.GetIM(IM_BuildingData));
	m_pGrndCtrlIMWidget->SetIM(g_IMSysMgr.GetIM(IM_GroundControl));

}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitIMView()
{
	IMViewWidget	*pIMView = NULL;
	QGridLayout		*gridLayout;

	// 1. Init Building Data IM
	pIMView = new IMViewWidget;
	m_pBuildingDataIMWidget= pIMView;
	m_IMViews.push_back(pIMView);

	gridLayout = new QGridLayout;
	ui.tbBuildingIM->setLayout(gridLayout);
	ui.tbBuildingIM->layout()->setAlignment(Qt::AlignCenter);
	ui.tbBuildingIM->layout()->addWidget(pIMView);
	ui.tbBuildingIM->layout()->setMargin(0);
	ui.tbBuildingIM->layout()->setSpacing(0);

	// 2. Init Ground Control IM
	pIMView = new IMViewWidget;
	m_pGrndCtrlIMWidget = pIMView;
	m_IMViews.push_back(pIMView);

	gridLayout = new QGridLayout;
	ui.tbGrndCtrlIM->setLayout(gridLayout);
	ui.tbGrndCtrlIM->layout()->setAlignment(Qt::AlignCenter);
	ui.tbGrndCtrlIM->layout()->addWidget(pIMView);
	ui.tbGrndCtrlIM->layout()->setMargin(0);
	ui.tbGrndCtrlIM->layout()->setSpacing(0);
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
	if (!m_bClientInitialized)
		InitClient();

	QMainWindow::showEvent(pEvent);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::closeEvent(QCloseEvent *pEvent) 
{
	Shutdown();
	QMainWindow::closeEvent(pEvent);
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::Shutdown()
{
	DWORD dwWait;

	g_bShutdown = true;

	printf("Shutting down ...\n");

	dwWait = WaitForSingleObject(m_hBwapiThread, INFINITE);
	assert(dwWait == WAIT_OBJECT_0);

	CloseHandle(m_hBwapiThread);
}
//////////////////////////////////////////////////////////////////////////
DWORD ClientMain::BwapiThreadStart(PVOID p_pvContext)
{
	ClientMain *pClient = (ClientMain*)p_pvContext;

	if (pClient)
	{
		try
		{
			pClient->BwapiMainThread();
		}
		catch(...)
		{
			printf("IStrategizer unhandled exception\nTerminating...\n");
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::ConnectToBwapiServer()
{
	while(!g_bShutdown && !BWAPIClient.connect())
	{
		Sleep(1000);
	}
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::BwapiMainThread()
{
	BWAPI::BWAPI_init();

	while(!g_bShutdown)
	{
		printf("Connecting...\n");
		ConnectToBwapiServer();

		printf("waiting to enter match\n");
		while (!g_bShutdown && !Broodwar->isInGame())
		{
			BWAPI::BWAPIClient.update();
			if (!BWAPI::BWAPIClient.isConnected())
			{
				printf("Reconnecting...\n");
				ConnectToBwapiServer();
			}
		}

		if (g_bShutdown) continue;

		printf("starting match!");
		Broodwar->sendText("Hello world!");
		Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
		// Enable some cheat flags
		Broodwar->enableFlag(Flag::UserInput);
		Broodwar->sendText("operation cwal");
		Broodwar->sendText("show me the money");

		// Uncomment to enable complete map information
		Broodwar->enableFlag(Flag::CompleteMapInformation);

		//read map information into BWTA so terrain analysis can be done in another thread
		BWTA::readMap();
		analyzed=false;
		analysis_just_finished=false;

		show_bullets=false;
		show_visibility_data=false;

		InitIStrategizer();
		
		if (Broodwar->isReplay())
		{
			Broodwar->printf("The following players are in this replay:");
			for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
			{
				if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
				{
					Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
				}
			}
		}
		else
		{
			Broodwar->printf("The match up is %s v %s",
				Broodwar->self()->getRace().getName().c_str(),
				Broodwar->enemy()->getRace().getName().c_str());

			InitResourceManager();
		}

		EventLoop();

		FinalizeIStrategizer();

		// Give IM widgets a chance to clear its buffer and redraw
		UpdateIMViews();

		if (g_bShutdown) continue;

		printf("Game ended\n");
	}
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::EventLoop()
{
	m_engineCycles = 0;

	while(!g_bShutdown && Broodwar->isInGame())
	{
		for(std::list<Event>::iterator e = Broodwar->getEvents().begin();
			!g_bShutdown && e != Broodwar->getEvents().end(); e++)
		{
			switch(e->getType())
			{
			//case EventType::MatchEnd:
			//	if (e->isWinner())
			//		printf("I won the game\n");
			//	else
			//		printf("I didn't win the game\n");
			//	break;
		
			//case EventType::ReceiveText:
			//	Broodwar->printf("%s said '%s'", e->getPlayer()->getName().c_str(), e->getText().c_str());
			//	break;
			//case EventType::PlayerLeft:
			//	Broodwar->sendText("%s left the game.",e->getPlayer()->getName().c_str());
			//	break;
			//case EventType::NukeDetect:
			//	if (e->getPosition()!=Positions::Unknown)
			//		Broodwar->printf("Nuclear Launch Detected at (%d,%d)",e->getPosition().x(),e->getPosition().y());
			//	else
			//		Broodwar->printf("Nuclear Launch Detected");
			//	break;
			//case EventType::UnitDiscover:
			//	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
			//		Broodwar->sendText("A %s [%x] has been discovered at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x(),e->getUnit()->getPosition().y());
			//	break;
			//case EventType::UnitEvade:
			//	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
			//		Broodwar->sendText("A %s [%x] was last accessible at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x(),e->getUnit()->getPosition().y());
			//	break;
			//case EventType::UnitShow:
			//	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
			//		Broodwar->sendText("A %s [%x] has been spotted at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x(),e->getUnit()->getPosition().y());
			//	break;
			//case EventType::UnitHide:
			//	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
			//		Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x(),e->getUnit()->getPosition().y());
			//	break;
			case EventType::UnitMorph:
				if (!Broodwar->isReplay())
					Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",e->getUnit()->getType().getName().c_str(),e->getUnit(),e->getUnit()->getPosition().x(),e->getUnit()->getPosition().y());
				else
				{
					/*if we are in a replay, then we will print out the build order
					(just of the buildings, not the units).*/
					if (e->getUnit()->getType().isBuilding() && e->getUnit()->getPlayer()->isNeutral()==false)
					{
						int seconds=Broodwar->getFrameCount()/24;
						int minutes=seconds/60;
						seconds%=60;
						Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,e->getUnit()->getPlayer()->getName().c_str(),e->getUnit()->getType().getName().c_str());
					}
				}
				break;
			case EventType::UnitRenegade:
				OnUniRenegade(e->getUnit());
				break;
			//case EventType::SaveGame:
			//	Broodwar->sendText("The game was saved to \"%s\".",e->getText().c_str());
			//	break;
			case EventType::UnitCreate:
				OnUnitCreate(e->getUnit());
				break;
			case EventType::UnitDestroy:
				OnUnitDestroy(e->getUnit());
				break;
			case EventType::SendText:
				OnSendText(e->getText());
				break;
			}
		}

		if (g_bShutdown) continue;

		static bool enemyPlayerCollected = false;

		if (!enemyPlayerCollected)
		{
			// This to solve the bug that the game does not send  messages about creating enemy units at game start
			TID enemyPlayerID = g_Database.PlayerMapping.GetBySecond(PLAYER_Enemy);
			const set<BWAPI::Unit*> &enemyUnits = Broodwar->getPlayer(enemyPlayerID)->getUnits();

			for (set<BWAPI::Unit*>::const_iterator itr = enemyUnits.begin();
				itr != enemyUnits.end(); ++itr)
			{
				OnUnitCreate(*itr);
			}

			enemyPlayerCollected = !enemyUnits.empty();
		}

		m_pIStrategizer->Update(Broodwar->getFrameCount());
		++m_engineCycles;

		UpdateIMViews();
		UpdateStatsView();

		// UpdateTerrainAnalysis();
		BWAPI::BWAPIClient.update();

		if (!BWAPI::BWAPIClient.isConnected())
			break;
	}
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnSendText(const string& p_text)
{
	if (p_text=="/show bullets")
	{
		show_bullets = !show_bullets;
	} else if (p_text=="/show players")
	{
		showPlayers();
	} else if (p_text=="/show forces")
	{
		showForces();
	} else if (p_text=="/show visibility")
	{
		show_visibility_data=!show_visibility_data;
	} else if (p_text=="/analyze")
	{
		if (analyzed == false)
		{
			Broodwar->printf("Analyzing map... this may take a minute");
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
		}
	} else
	{
		Broodwar->printf("[IStrategizer]You typed '%s'!",p_text.c_str());

		TextMessage *pTxtMsg;

		pTxtMsg = new TextMessage(Broodwar->getFrameCount(), MSG_Input, new string(p_text));
		g_MessagePump.Send(pTxtMsg);
	}
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::OnUnitCreate(BWAPI::Unit *p_pUnit)
{
	//if (Broodwar->getFrameCount()>2)
	//{
	//	if (!Broodwar->isReplay() &&  p_pUnit->getPlayer()->isEnemy(Broodwar->self()))
	//	{
	//		Broodwar->sendText("A %s [%x] has been created at (%d,%d)",p_pUnit->getType().getName().c_str(),p_pUnit,p_pUnit->getPosition().x(),p_pUnit->getPosition().y());
	//	}
	//	else
	//	{
	//		/*if we are in a replay, then we will print out the build order
	//		(just of the buildings, not the units).*/
	//		if (p_pUnit->getType().isBuilding() && p_pUnit->getPlayer()->isNeutral()==false)
	//		{
	//			int seconds=Broodwar->getFrameCount()/24;
	//			int minutes=seconds/60;
	//			seconds%=60;
	//			Broodwar->sendText("%.2d:%.2d: %s creates a %s",minutes,seconds,p_pUnit->getPlayer()->getName().c_str(),p_pUnit->getType().getName().c_str());
	//		}
	//	}
	//}

	EntityMessageData	*pData = NULL;
	EntityCreateMessage	*pMsg = NULL;

	pData = new EntityMessageData;
	assert(pData);

	assert(p_pUnit);
	pData->EntityId = p_pUnit->getID();
	pData->OwnerId = g_Database.PlayerMapping.GetByFirst(p_pUnit->getPlayer()->getID());

	if (p_pUnit->getType().isBuilding())
	{
		pData->X = UnitPositionFromTilePosition(p_pUnit->getTilePosition().x());
		pData->Y = UnitPositionFromTilePosition(p_pUnit->getTilePosition().y());
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
void ClientMain::OnUnitDestroy(BWAPI::Unit *p_pUnit)
{
	//if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
	//	Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",p_pUnit->getType().getName().c_str(),p_pUnit,p_pUnit->getPosition().x(),p_pUnit->getPosition().y());

	EntityMessageData		*pData = NULL;
	EntityDestroyMessage	*pMsg = NULL;

	pData = new EntityMessageData;
	assert(pData);

	assert(p_pUnit);

	pData->EntityId = p_pUnit->getID();
	pData->OwnerId = g_Database.PlayerMapping.GetByFirst(p_pUnit->getPlayer()->getID());

	if (p_pUnit->getType().isBuilding())
	{
		pData->X = UnitPositionFromTilePosition(p_pUnit->getTilePosition().x());
		pData->Y = UnitPositionFromTilePosition(p_pUnit->getTilePosition().y());
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
void ClientMain::OnUniRenegade(BWAPI::Unit *p_pUnit)
{
	//if (!Broodwar->isReplay())
	//	Broodwar->sendText("A %s:%d is now owned by %s", p_pUnit->getType().getName().c_str(), p_pUnit->getID(), p_pUnit->getPlayer()->getName().c_str());

	EntityMessageData		*pData = NULL;
	EntityRenegadeMessage	*pMsg = NULL;

	pData = new EntityMessageData;
	assert(pData);

	assert(p_pUnit);
	pData->EntityId = p_pUnit->getID();
	pData->OwnerId = g_Database.PlayerMapping.GetByFirst(p_pUnit->getPlayer()->getID());

	if (p_pUnit->getType().isBuilding())
	{
		pData->X = UnitPositionFromTilePosition(p_pUnit->getTilePosition().x());
		pData->Y = UnitPositionFromTilePosition(p_pUnit->getTilePosition().y());
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
void ClientMain::UpdateTerrainAnalysis()
{
	if (show_bullets)
		drawBullets();

	if (show_visibility_data)
		drawVisibilityData();

	if (!Broodwar->isReplay())
	{
		drawStats();

		if (analyzed && Broodwar->getFrameCount()%30==0)
		{
			//order one of our workers to guard our chokepoint.
			for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
			{
				if ((*i)->getType().isWorker())
				{
					//get the chokepoints linked to our home region
					std::set<BWTA::Chokepoint*> chokepoints= home->getChokepoints();
					double min_length=10000;
					BWTA::Chokepoint* choke=NULL;

					//iterate through all chokepoints and look for the one with the smallest gap (least width)
					for(std::set<BWTA::Chokepoint*>::iterator c=chokepoints.begin();c!=chokepoints.end();c++)
					{
						double length=(*c)->getWidth();
						if (length<min_length || choke==NULL)
						{
							min_length=length;
							choke=*c;
						}
					}

					//order the worker to move to the center of the gap
					(*i)->rightClick(choke->getCenter());
					break;
				}
			}
		}
		if (analysis_just_finished)
		{
			Broodwar->printf("Finished analyzing map.");
			analysis_just_finished=false;
		}
		if (analyzed)
			drawTerrainData();
	}
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::UpdateStatsView()
{
	float engineRatio, gameRatio;

	ui.lblEngineCycleData->setText(tr("%1").arg(m_engineCycles));
	ui.lblGameCyclesData->setText(tr("%1").arg(Broodwar->getFrameCount()));

	engineRatio = (float)Broodwar->getFrameCount();
	gameRatio = (float)m_engineCycles;

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

}
//////////////////////////////////////////////////////////////////////////
void ClientMain::UpdateIMViews()
{
	for (int i = 0, size = m_IMViews.size(); i < size; ++i)
		m_IMViews[i]->update();
}
//////////////////////////////////////////////////////////////////////////
void ClientMain::InitResourceManager()
{
	//send each worker to the mineral field that is closest to it
	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if ((*i)->getType().isWorker())
		{
			Unit* closestMineral=NULL;
			for(std::set<Unit*>::iterator m=Broodwar->getMinerals().begin();m!=Broodwar->getMinerals().end();m++)
			{
				if (closestMineral==NULL || (*i)->getDistance(*m)<(*i)->getDistance(closestMineral))
					closestMineral=*m;
			}
			if (closestMineral!=NULL)
				(*i)->rightClick(closestMineral);
		}
		else if ((*i)->getType().isResourceDepot())
		{
			//if this is a center, tell it to build the appropiate type of worker
			if ((*i)->getType().getRace()!=Races::Zerg)
			{
				(*i)->train(Broodwar->self()->getRace().getWorker());
			}
			else //if we are Zerg, we need to select a larva and morph it into a drone
			{
				std::set<Unit*> myLarva=(*i)->getLarva();
				if (myLarva.size()>0)
				{
					Unit* larva=*myLarva.begin();
					larva->morph(UnitTypes::Zerg_Drone);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();

	//self start location only available if the map has base locations
	if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
	{
		home       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
	}
	//enemy start location only available if Complete Map Information is enabled.
	if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
	{
		enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
	}
	analyzed   = true;
	analysis_just_finished = true;
	return 0;
}

void drawStats()
{
	std::set<Unit*> myUnits = Broodwar->self()->getUnits();
	Broodwar->drawTextScreen(5,0,"I have %d units:",myUnits.size());
	std::map<UnitType, int> unitTypeCounts;
	for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
	{
		if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
		{
			unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
		}
		unitTypeCounts.find((*i)->getType())->second++;
	}
	int line=1;
	for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
	{
		Broodwar->drawTextScreen(5,16*line,"- %d %ss",(*i).second, (*i).first.getName().c_str());
		line++;
	}
}

void drawBullets()
{
	std::set<Bullet*> bullets = Broodwar->getBullets();
	for(std::set<Bullet*>::iterator i=bullets.begin();i!=bullets.end();i++)
	{
		Position p=(*i)->getPosition();
		double velocityX = (*i)->getVelocityX();
		double velocityY = (*i)->getVelocityY();
		if ((*i)->getPlayer()==Broodwar->self())
		{
			Broodwar->drawLineMap(p.x(),p.y(),p.x()+(int)velocityX,p.y()+(int)velocityY,Colors::Green);
			Broodwar->drawTextMap(p.x(),p.y(),"\x07%s",(*i)->getType().getName().c_str());
		}
		else
		{
			Broodwar->drawLineMap(p.x(),p.y(),p.x()+(int)velocityX,p.y()+(int)velocityY,Colors::Red);
			Broodwar->drawTextMap(p.x(),p.y(),"\x06%s",(*i)->getType().getName().c_str());
		}
	}
}

void drawVisibilityData()
{
	for(int x=0;x<Broodwar->mapWidth();x++)
	{
		for(int y=0;y<Broodwar->mapHeight();y++)
		{
			if (Broodwar->isExplored(x,y))
			{
				if (Broodwar->isVisible(x,y))
					Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Green);
				else
					Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Blue);
			}
			else
				Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Red);
		}
	}
}

void drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
	{
		TilePosition p=(*i)->getTilePosition();
		Position c=(*i)->getPosition();

		//draw outline of center location
		Broodwar->drawBox(CoordinateType::Map,p.x()*32,p.y()*32,p.x()*32+4*32,p.y()*32+3*32,Colors::Blue,false);

		//draw a circle at each mineral patch
		for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
		{
			Position q=(*j)->getInitialPosition();
			Broodwar->drawCircle(CoordinateType::Map,q.x(),q.y(),30,Colors::Cyan,false);
		}

		//draw the outlines of vespene geysers
		for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();j++)
		{
			TilePosition q=(*j)->getInitialTilePosition();
			Broodwar->drawBox(CoordinateType::Map,q.x()*32,q.y()*32,q.x()*32+4*32,q.y()*32+2*32,Colors::Orange,false);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if ((*i)->isIsland())
			Broodwar->drawCircle(CoordinateType::Map,c.x(),c.y(),80,Colors::Yellow,false);
	}

	//we will iterate through all the regions and draw the polygon outline of it in green.
	for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
	{
		BWTA::Polygon p=(*r)->getPolygon();
		for(int j=0;j<(int)p.size();j++)
		{
			Position point1=p[j];
			Position point2=p[(j+1) % p.size()];
			Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Green);
		}
	}

	//we will visualize the chokepoints with red lines
	for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
	{
		for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
		{
			Position point1=(*c)->getSides().first;
			Position point2=(*c)->getSides().second;
			Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Red);
		}
	}
}

void showPlayers()
{
	std::set<Player*> players=Broodwar->getPlayers();
	for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
	{
		Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
	}
}
//////////////////////////////////////////////////////////////////////////
void showForces()
{
	std::set<Force*> forces=Broodwar->getForces();
	for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
	{
		std::set<Player*> players=(*i)->getPlayers();
		Broodwar->printf("Force %s has the following players:",(*i)->getName().c_str());
		for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
		{
			Broodwar->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
		}
	}
}

#include "YarmoukAIModule.h"

#include "DataMessage.h"
#include "MessagePump.h"
#include "DefinitionCrossMapping.h"
#include "StarCraftGame.h"
#include "IMSystemManager.h"
#include "InfluenceMap.h"
#include "GamePlayer.h"
#include "StarCraftEntity.h"
#include "StarCraftStrategySelector.h"
#include "Console.h"
#include <iostream>

using namespace std;
using namespace BWAPI;
using namespace IStrategizer;

CConsole console;

void YarmoukAIModule::onStart()
{
    // Print the map name.
    // BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
    Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

    // Enable the UserInput flag, which allows us to control the bot and type messages.
    Broodwar->enableFlag(Flag::UserInput);

    // Uncomment the following line and the bot will know about everything through the fog of war (cheat).
    //Broodwar->enableFlag(Flag::CompleteMapInformation);

    // Set the command optimization level so that common commands can be grouped
    // and reduce the bot's APM (Actions Per Minute).
    Broodwar->setCommandOptimizationLevel(2);

    // Retrieve you and your enemy's races. enemy() will just return the first enemy.
    // If you wish to deal with multiple enemies then you must use enemies().
    if (Broodwar->enemy()) // First make sure there is an enemy
        Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;

    IStrategizer::IStrategizerEx::sm_WorkingDir = AIIDE_IO_DIR;
    g_SerializationSystemWorkingDir = AIIDE_IO_DIR;
    IStrategizer::Init();

    FinalizeIStrategizer();

    // Dump memory leaks if exist
    EngineObject::DumpAliveObjects();
    // Make sure that we start from a clean slate

    // Hard Reset is always a bad idea and can cause inconsistency
    // EngineObject::FreeMemoryPool();

    InitIStrategizer();

    Message *pMsg;

    pMsg = new Message(Broodwar->getFrameCount(), MSG_GameStart);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg);
}

void YarmoukAIModule::onEnd(bool isWinner)
{
    GameEndMessageData    *pData = nullptr;
    GameEndMessage        *pMsg = nullptr;

    pData = new GameEndMessageData;
    _ASSERTE(pData);

    Player player = Broodwar->getPlayer(g_Database.PlayerMapping.GetBySecond(PLAYER_Self));
    pData->IsWinner = isWinner;
    pData->Score = player->getBuildingScore() + player->getRazingScore() + player->getUnitScore() + player->getCustomScore() + player->getKillScore();
    pData->MapName = Broodwar->mapFileName();
    pData->EnemyRace = g_Game->GetRace(Broodwar->getPlayer(g_Database.PlayerMapping.GetBySecond(PLAYER_Enemy))->getRace().getID())->ToString();
    pMsg = new GameEndMessage(Broodwar->getFrameCount(), MSG_GameEnd, pData);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg);
}

void YarmoukAIModule::onFrame()
{
    // Called once every game frame

    // Display the game frame rate as text in the upper left area of the screen
    Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
    Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());

    // Return if the game is a replay or is paused
    if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
        return;

    // Prevent spamming by only running our onFrame once every number of latency frames.
    // Latency frames are the number of frames before commands are processed.
    if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
        return;

    try
    {
        m_pIStrategizer->Update(Broodwar->getFrameCount());
    }
    catch (IStrategizer::Exception &e)
    {
        e.To(cout);
    }
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::InitIStrategizer()
{
    IStrategizerParam param;

    try
    {
        m_pGameModel = new StarCraftGame;
        _ASSERTE(m_pGameModel);

        param.OccupanceIMCellSize = TILE_SIZE;
        param.GrndCtrlIMCellSize = TILE_SIZE;
        param.OccupanceIMUpdateInterval = 1;
        param.GrndCtrlIMUpdateInterval = 32;
        param.Consultant = make_shared<StarCraftStrategySelector>();

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
    }
    catch (IStrategizer::Exception& e)
    {
        e.To(cout);
    }
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::FinalizeIStrategizer()
{
    SAFE_DELETE(m_pIStrategizer);

    RtsGame::FinalizeStaticData();
    SAFE_DELETE(m_pGameModel);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onUnitCreate(BWAPI::Unit p_pUnit)
{
    LogInfo("OnUnitCreate -> %s[%d]", p_pUnit->getType().toString().c_str(), p_pUnit->getID());

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
void YarmoukAIModule::onUnitDestroy(BWAPI::Unit p_pUnit)
{
    LogInfo("OnUnitDestroy -> %s[%d]", p_pUnit->getType().toString().c_str(), p_pUnit->getID());

    EntityMessageData *pData = nullptr;
    EntityDestroyMessage    *pMsg = nullptr;

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

    pMsg = new EntityDestroyMessage(Broodwar->getFrameCount(), MSG_EntityDestroy, pData);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg, true);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onUnitRenegade(BWAPI::Unit p_pUnit)
{
    LogInfo("OnUnitRenegade -> %s[%d]", p_pUnit->getType().toString().c_str(), p_pUnit->getID());

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
void YarmoukAIModule::onUnitShow(BWAPI::Unit p_pUnit)
{
    LogInfo("OnUnitShow -> %s[%d]", p_pUnit->getType().toString().c_str(), p_pUnit->getID());

    EntityMessageData* pData = nullptr;
    EntityShowMessage* pMsg = nullptr;

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

    pMsg = new EntityShowMessage(Broodwar->getFrameCount(), MSG_EntityShow, pData);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onUnitHide(BWAPI::Unit p_pUnit)
{
    LogInfo("OnUnitHide -> %s[%d] @ Frame %d", p_pUnit->getType().toString().c_str(), p_pUnit->getID(), Broodwar->getFrameCount());

    EntityMessageData* pData = nullptr;
    EntityHideMessage* pMsg = nullptr;

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

    pMsg = new EntityHideMessage(Broodwar->getFrameCount(), MSG_EntityHide, pData);
    _ASSERTE(pMsg);

    g_MessagePump->Send(pMsg, true);
}

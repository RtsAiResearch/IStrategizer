#include "YarmoukAIModule.h"
#include "IStrategizer.h"
#include "EngineDefs.h"
#include "DefinitionCrossMapping.h"
#include <iostream>

using namespace std;
using namespace BWAPI;
using namespace IStrategizer;

void YarmoukAIModule::onStart()
{
    // Print the map name.
    // BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
    Broodwar << "The map is " << Broodwar->mapName().c_str() << "!" << std::endl;

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

    InitIStrategizer();

    if (m_pAiEngine)
        m_pAiEngine->SendEngineMessage(MSG_GameStart);
}

void YarmoukAIModule::onEnd(bool isWinner)
{
    // FIXME:
    //GameEndMessageData    *pData = nullptr;
    //GameEndMessage        *pMsg = nullptr;

    //pData = new GameEndMessageData;
    //_ASSERTE(pData);

    //Player player = Broodwar->getPlayer(g_Database.PlayerMapping.GetBySecond(PLAYER_Self));
    //pData->IsWinner = isWinner;
    //pData->Score = player->getBuildingScore() + player->getRazingScore() + player->getUnitScore() + player->getCustomScore() + player->getKillScore();
    //pData->MapName = Broodwar->mapFileName();
    //pData->EnemyRace = g_Game->GetRace(Broodwar->getPlayer(g_Database.PlayerMapping.GetBySecond(PLAYER_Enemy))->getRace().getID())->ToString();
    //pMsg = new GameEndMessage(Broodwar->getFrameCount(), MSG_GameEnd, pData);
    //_ASSERTE(pMsg);

    //g_MessagePump->Send(pMsg);
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

    m_pAiEngine->Update();
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::InitIStrategizer()
{
    EngineParams param;

    try
    {
        // FIXME
        m_pGameModel = nullptr;
        m_pGameModel->Init();

        param.OccupanceIMCellSize = TILE_SIZE;
        param.GrndCtrlIMCellSize = TILE_SIZE;
        param.OccupanceIMUpdateInterval = 1;
        param.GrndCtrlIMUpdateInterval = 32;

        if (Broodwar->isReplay())
        {
            Broodwar->sendText("Watching replay map: %s", Broodwar->mapFileName().c_str());
            param.Phase = PHASE_Offline;
        }
        else
            param.Phase = PHASE_Online;

        Broodwar->setLocalSpeed(0);

        m_pAiEngine = GetRtsAiEngineFactory()->CreateEngine(param, m_pGameModel);
        _ASSERTE(m_pAiEngine);
        m_pAiEngine->SetEngineReadWriteDir("bwapi-data\\AI\\", "bwapi-data\\AI\\");

        g_Database.Init();

        if (!m_pAiEngine->Init())
        {
            return;
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
    GetRtsAiEngineFactory()->DestroyEngine(m_pAiEngine);
    m_pAiEngine = nullptr;

    RtsAiEngineSystemDeinit();
    SAFE_DELETE(m_pGameModel);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::OnEntityMessage(BWAPI::Unit pUnit, MessageType msgType)
{
    EntityMessageData data;

    data.EntityId = pUnit->getID();
    data.OwnerId = g_Database.PlayerMapping.GetByFirst(pUnit->getPlayer()->getID());
    data.EntityType = g_Database.EntityMapping.GetByFirst(pUnit->getType());

    if (pUnit->getType().isBuilding())
    {
        data.X = pUnit->getTilePosition().x * TILE_SIZE;
        data.Y = pUnit->getTilePosition().y * TILE_SIZE;
    }
    else
    {
        data.X = pUnit->getLeft();
        data.Y = pUnit->getTop();
    }

    m_pAiEngine->SendEngineEntityMessage(msgType, data);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onUnitCreate(BWAPI::Unit pUnit)
{
    OnEntityMessage(pUnit, MSG_EntityCreate);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onUnitDestroy(BWAPI::Unit pUnit)
{
    OnEntityMessage(pUnit, MSG_EntityDestroy);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onUnitRenegade(BWAPI::Unit pUnit)
{
    OnEntityMessage(pUnit, MSG_EntityRenegade);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onUnitShow(BWAPI::Unit pUnit)
{
    OnEntityMessage(pUnit, MSG_EntityShow);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onUnitHide(BWAPI::Unit pUnit)
{
    OnEntityMessage(pUnit, MSG_EntityHide);
}
//////////////////////////////////////////////////////////////////////////
void YarmoukAIModule::onSendText(std::string text)
{
    static const char* commands[] = {
        "export-all-ids",
        "export-game-ids",
        "export-statics"
    };

    unsigned cmdLen;
    const char* cmdParam;

    if (!strncmp(text.c_str(), commands[0], strlen(commands[0])))
    {
        cmdLen = strlen(commands[0]);
        cmdParam = text.c_str() + cmdLen + 1;

        if (g_Database.ExportAllIds(cmdParam))
            Broodwar->sendText("All ids exported successfully");
        else
            Broodwar->sendText("Failed to export all ids");
    }
    else if (!strncmp(text.c_str(), commands[1], strlen(commands[1])))
    {
        cmdLen = strlen(commands[1]);
        cmdParam = text.c_str() + cmdLen + 1;

        if (g_Database.ExportGameIds(cmdParam))
            Broodwar->sendText("Game ids exported successfully");
        else
            Broodwar->sendText("Failed to export game ids");
    }
}
//////////////////////////////////////////////////////////////////////////
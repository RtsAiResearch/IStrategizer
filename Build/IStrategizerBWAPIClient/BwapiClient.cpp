#include "BwapiClient.h"
#include <cstdio>
#include <cassert>
#include <set>
#include "CmnHdr.h"

using namespace std;
using namespace BWAPI;

BwapiClient::BwapiClient() : m_bShutdown(false), m_hBwapiThread(nullptr), m_bClientInitialized(false) {}
//////////////////////////////////////////////////////////////////////////
void BwapiClient::InitClient()
{
    if (m_bClientInitialized)
        return;

    m_hBwapiThread = chBEGINTHREADEX(nullptr, 0, BwapiThreadStart, this, 0, nullptr);
    _ASSERTE(m_hBwapiThread);

    if (m_hBwapiThread)
        m_bClientInitialized = true;
}
//////////////////////////////////////////////////////////////////////////
DWORD BwapiClient::BwapiThreadStart(PVOID p_pvContext)
{
    BwapiClient *pClient = (BwapiClient*)p_pvContext;

    if (pClient)
        pClient->BwapiMainThread();

    return 0;
}
//////////////////////////////////////////////////////////////////////////
void BwapiClient::BwapiMainThread()
{

    while(!m_bShutdown)
    {
        printf("Connecting...\n");
        ConnectToBwapiServer();

        printf("Waiting to enter match...\n");

        while (!m_bShutdown && !Broodwar->isInGame())
        {
            BWAPI::BWAPIClient.update();

            if (!BWAPI::BWAPIClient.isConnected())
            {
                printf("Reconnecting...\n");
                ConnectToBwapiServer();
            }
        }

        if (m_bShutdown) continue;

        printf("Starting match!\n");

        Broodwar->sendText("Hello world!");
        Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());

        // Enable some cheat flags
        Broodwar->enableFlag(Flag::UserInput);
        // Uncomment to enable complete map information
        Broodwar->enableFlag(Flag::CompleteMapInformation);

        if (Broodwar->isReplay())
        {
            Broodwar->printf("The following players are in this replay:");
            for(BWAPI::Playerset::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
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
        }

        OnClientLoopStart();
        ClientLoop();
        OnClientLoopEnd();

        if (m_bShutdown) continue;

        printf("Game ended\n");
    }
}
//////////////////////////////////////////////////////////////////////////
void BwapiClient::ShutdownClient()
{
    DWORD dwWait;

    m_bShutdown = true;

    printf("Shutting down ...\n");

    dwWait = WaitForSingleObject(m_hBwapiThread, INFINITE);
    _ASSERTE(dwWait == WAIT_OBJECT_0);

    CloseHandle(m_hBwapiThread);
}
//////////////////////////////////////////////////////////////////////////
void BwapiClient::ConnectToBwapiServer()
{
    while(!m_bShutdown && !BWAPIClient.connect())
    {
        Sleep(250);
    }
}
//////////////////////////////////////////////////////////////////////////
void BwapiClient::ClientLoop()
{
    while(!m_bShutdown && Broodwar->isInGame())
    {
        HandleGameEvents();

        if (m_bShutdown) continue;

        OnClientUpdate();

        BWAPI::BWAPIClient.update();

        if (!BWAPI::BWAPIClient.isConnected())
            break;
    }
}
//////////////////////////////////////////////////////////////////////////
void BwapiClient::HandleGameEvents()
{
    for(std::list<BWAPI::Event>::const_iterator e = Broodwar->getEvents().begin();
        !m_bShutdown && e != Broodwar->getEvents().end(); e++)
    {
        switch(e->getType())
        {
        case EventType::MatchFrame:
            OnGameFrame();
            break;
        case EventType::MatchStart:
            OnMatchStart();
            break;
        case EventType::MatchEnd:
            OnMatchEnd(e->isWinner());
            break;
        case EventType::UnitMorph:
            OnUnitMorph(e->getUnit());
            break;
        case EventType::UnitRenegade:
            OnUnitRenegade(e->getUnit());
            break;
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
}

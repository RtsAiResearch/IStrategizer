#define LOADERAPI extern "C" __declspec(dllexport)

#include "AIModuleLoader.h"
#include <cstdio>
#include <cassert>
#include <set>
#include <iostream>
#include <process.h>
#include "BWAPI.h"
#include "BWAPI/Client.h"

using namespace std;
using namespace BWAPI;

// This macro function calls the C runtime's _beginthreadex function. 
// The C runtime library doesn't want to have any reliance on Windows' data 
// types such as HANDLE. This means that a Windows programmer needs to cast
// values when using _beginthreadex. Since this is terribly inconvenient, 
// I created this macro to perform the casting.
typedef unsigned(__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
    pvParam, dwCreateFlags, pdwThreadId)                 \
    ((HANDLE)_beginthreadex(\
    (void *)(psa), \
    (unsigned)(cbStackSize), \
    (PTHREAD_START)(pfnStartAddr), \
    (void *)(pvParam), \
    (unsigned)(dwCreateFlags), \
    (unsigned *)(pdwThreadId)))


static PfnMatchStartCallback g_pfnMatchStartCallback = nullptr;
static PfnMatchEndCallback g_pfnMatchEndCallback = nullptr;

class BwapiClient
{
public:
    void BwapiMainThread();
    BwapiClient() :
    m_bShutdown(false),
        m_hBwapiThread(nullptr),
        m_bClientInitialized(false),
        m_botDllFilename(""),
        m_pAiModule(nullptr),
        m_bModuleLoaded(false)
    {}

    void InitClient(const char* pBotDllFilename);
    bool ClientInitialized();
    void ClientLoop();
    void HandleGameEvents();
    void ConnectToBwapiServer();
    virtual void ShutdownClient();
    virtual void OnClientLoopStart() {};
    virtual void OnClientLoopEnd() {};
    virtual void OnUnitCreate(BWAPI::Unit pUnit) { m_pAiModule->onUnitCreate(pUnit); }
    virtual void OnUnitDestroy(BWAPI::Unit pUnit) { m_pAiModule->onUnitDestroy(pUnit); }
    virtual void OnUnitRenegade(BWAPI::Unit pUnit) { m_pAiModule->onUnitRenegade(pUnit); }
    virtual void OnUnitMorph(BWAPI::Unit pUnit) { m_pAiModule->onUnitMorph(pUnit); }
    virtual void OnUnitShow(BWAPI::Unit pUnit) { m_pAiModule->onUnitShow(pUnit); }
    virtual void OnUnitHide(BWAPI::Unit pUnit) { m_pAiModule->onUnitHide(pUnit); }
    virtual void OnGameFrame() { m_pAiModule->onFrame(); }
    virtual void OnMatchStart() { m_pAiModule->onStart(); if (g_pfnMatchEndCallback) g_pfnMatchStartCallback(); };
    virtual void OnMatchEnd(bool isWinner) { m_pAiModule->onEnd(isWinner); if (g_pfnMatchEndCallback) g_pfnMatchEndCallback(isWinner); }
    virtual void OnSendText(const std::string &p_text) { m_pAiModule->onSendText(p_text); }
    virtual BWAPI::AIModule* GetAiModule() { return m_pAiModule; }
    static DWORD BwapiThreadStart(PVOID p_pvContext);

    HANDLE m_hBwapiThread;
    bool m_bClientInitialized;
    volatile bool m_bShutdown;
    std::string m_botDllFilename;
    BWAPI::AIModule* m_pAiModule;
    bool m_bModuleLoaded;
};

static BwapiClient g_Loader;
static HMODULE g_hMod;
LOADERAPI void AiModuleLoaderInit(const char* pBotDllFilename)
{
    g_Loader.InitClient(pBotDllFilename);
}

LOADERAPI bool AiModuleLoaderInitialized()
{
    return g_Loader.ClientInitialized();
}

LOADERAPI void AiModuleLoaderShutdown()
{
    return g_Loader.ShutdownClient();
}

LOADERAPI void AiModuleLoaderSetMatchStartCallback(PfnMatchStartCallback pfn)
{
    g_pfnMatchStartCallback = pfn;
}

LOADERAPI void AiModuleLoaderSetMatchEndCallback(PfnMatchEndCallback pfn)
{
    g_pfnMatchEndCallback = pfn;
}

LOADERAPI void* AIModuleLoaderGetEngine()
{
    typedef void* (*PFNGetYarmoukEngine)(BWAPI::AIModule*);

    PFNGetYarmoukEngine getYarmoukEngine = (PFNGetYarmoukEngine)GetProcAddress(g_hMod, "getYarmoukEngine");
    return (void*)getYarmoukEngine(g_Loader.GetAiModule());
}

void BwapiClient::InitClient(const char* pBotDllFilename)
{
    if (m_bClientInitialized)
        return;

    m_botDllFilename = pBotDllFilename;

    m_hBwapiThread = chBEGINTHREADEX(nullptr, 0, BwapiClient::BwapiThreadStart, this, 0, nullptr);
    _ASSERTE(g_Loader.m_hBwapiThread);

    if (m_hBwapiThread)
        m_bClientInitialized = true;
}

bool BwapiClient::ClientInitialized()
{
    return m_bClientInitialized;
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
    std::cout << "Connecting..." << std::endl;

    assert(BWAPIClient.isConnected() == false);
    ConnectToBwapiServer();
    assert(BroodwarPtr != nullptr);

    while (!m_bShutdown)
    {
        std::cout << "waiting to enter match" << std::endl;
        while (!m_bShutdown && !Broodwar->isInGame())
        {
            //std::cout << "attempting update" << std::endl;
            BWAPI::BWAPIClient.update();
            if (!BWAPI::BWAPIClient.isConnected())
            {
                std::cout << "Reconnecting..." << std::endl;
                ConnectToBwapiServer();
            }
        }
        std::cout << "entered match" << std::endl;
        m_bModuleLoaded = false;
        g_hMod = LoadLibraryA(m_botDllFilename.c_str());
        if (g_hMod == NULL)
        {
            std::cerr << "ERROR: Failed to load the AI Module" << std::endl;
            m_pAiModule = new AIModule();
            Broodwar->sendText("Error: Failed to load the AI Module");
        }
        else
        {
            typedef AIModule* (*PFNCreateA1)();
            typedef void(*PFNGameInit)(Game *);

            PFNGameInit newGame = (PFNGameInit)GetProcAddress(g_hMod, "gameInit");
            PFNCreateA1 newAIModule = (PFNCreateA1)GetProcAddress(g_hMod, "newAIModule");

            if (!newGame || !newAIModule)
            {
                std::cerr << "ERROR: Failed to find AI Module exports" << std::endl;
                m_pAiModule = new AIModule();
                Broodwar->sendText("Error: Failed to find AI Module exports");
            }
            else
            {
                newGame(BroodwarPtr);
                m_pAiModule = newAIModule();
                m_bModuleLoaded = true;
            }
        }
        std::cout << "starting match!" << std::endl;

        OnClientLoopStart();
        while (!m_bShutdown && Broodwar->isInGame())
        {
            HandleGameEvents();

            BWAPI::BWAPIClient.update();
            if (!BWAPI::BWAPIClient.isConnected())
            {
                std::cout << "Reconnecting..." << std::endl;
                ConnectToBwapiServer();
            }
        }
        OnClientLoopEnd();

        if (m_bModuleLoaded)
        {
            typedef void(*PFNDeleteAI)(BWAPI::AIModule*);
            PFNDeleteAI deleteAIModule = (PFNDeleteAI)GetProcAddress(g_hMod, "deleteAIModule");
            deleteAIModule(m_pAiModule);
            m_pAiModule = nullptr;
        }
        else
            delete m_pAiModule;
        FreeLibrary(g_hMod);
        std::cout << "Game ended" << std::endl;
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

    m_bClientInitialized = false;
    m_botDllFilename.clear();
    m_bShutdown = false;
    m_bModuleLoaded = false;
    m_pAiModule = nullptr;
}
//////////////////////////////////////////////////////////////////////////
void BwapiClient::ConnectToBwapiServer()
{
    while (!m_bShutdown && !BWAPIClient.connect())
    {
        Sleep(1000);
    }
}
//////////////////////////////////////////////////////////////////////////
void BwapiClient::HandleGameEvents()
{
    for (std::list<BWAPI::Event>::const_iterator e = Broodwar->getEvents().begin();
        !m_bShutdown && e != Broodwar->getEvents().end(); e++)
    {
        switch (e->getType())
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
        case EventType::UnitShow:
            OnUnitShow(e->getUnit());
            break;
        case EventType::UnitHide:
            OnUnitHide(e->getUnit());
            break;;
        case EventType::SendText:
            OnSendText(e->getText());
            break;
        }
    }
}

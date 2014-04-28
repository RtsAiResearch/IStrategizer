#ifndef BWAPICLIENT_H
#define BWAPICLIENT_H

#include <Windows.h>
#include <string>

#include "BWAPI.h"
#include "BWAPI\Client.h"

class BwapiClient
{
public:
    void BwapiMainThread();
    bool ClientInitialized() const { return m_bClientInitialized; }

protected:
    BwapiClient();
    void ClientLoop();
    void HandleGameEvents();
    void ConnectToBwapiServer();
    virtual void ShutdownClient();
    virtual void InitClient();
    virtual void OnClientUpdate() = 0;
    virtual void OnClientLoopStart() {};
    virtual void OnClientLoopEnd() {};
    virtual void OnUnitCreate(BWAPI::Unit p_pUnit) {}
    virtual void OnUnitDestroy(BWAPI::Unit p_pUnit) {}
    virtual void OnUnitRenegade(BWAPI::Unit p_pUnit) {}
    virtual void OnUnitMorph(BWAPI::Unit p_pUnit) {}
    virtual void OnGameFrame() {}
    virtual void OnMatchStart() {};
    virtual void OnMatchEnd(bool p_isWinner) {}
    virtual void OnSendText(const std::string &p_text) {}
    static DWORD BwapiThreadStart(PVOID p_pvContext);

private:
    HANDLE m_hBwapiThread;
    bool m_bClientInitialized;
    volatile bool m_bShutdown;
};

#endif // BWAPICLIENT_H

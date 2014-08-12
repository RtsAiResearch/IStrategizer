#pragma once
#include "RtsAiEngine.h"
#include "BWAPI.h"
#include "BWAPI\Client.h"

#ifdef AIIDE
#define ENGINE_IO_DIR "bwapi-data\\AI\\"
#else
#define ENGINE_IO_DIR ".\\"
#endif

class YarmoukAIModule : public BWAPI::AIModule
{
public:
    YarmoukAIModule() :
        m_pAiEngine(nullptr),
        m_pGameModel(nullptr)
    {}

    // Virtual functions for callbacks, leave these as they are.
    virtual void onStart();
    virtual void onEnd(bool isWinner);
    virtual void onFrame();
    virtual void onSendText(std::string text);
    virtual void onReceiveText(BWAPI::Player player, std::string text) {}
    virtual void onPlayerLeft(BWAPI::Player player) {}
    virtual void onNukeDetect(BWAPI::Position target) {}
    virtual void onUnitDiscover(BWAPI::Unit unit) {}
    virtual void onUnitEvade(BWAPI::Unit unit) {}
    virtual void onUnitShow(BWAPI::Unit unit);
    virtual void onUnitHide(BWAPI::Unit unit);
    virtual void onUnitCreate(BWAPI::Unit unit);
    virtual void onUnitDestroy(BWAPI::Unit unit);
    virtual void onUnitMorph(BWAPI::Unit unit) {}
    virtual void onUnitRenegade(BWAPI::Unit unit);
    virtual void onSaveGame(std::string gameName) {}
    virtual void onUnitComplete(BWAPI::Unit unit) {}
    virtual IStrategizer::IRtsAiEngine* Engine() { return m_pAiEngine; }
    // Everything below this line is safe to modify.

protected:
    void OnEntityMessage(BWAPI::Unit unit, IStrategizer::MessageType msgType);
    void InitIStrategizer();
    void FinalizeIStrategizer();
    bool ExportGameIds(std::string p_exportPath);
    bool ExportAllIds(std::string p_exportPath);

    IStrategizer::IRtsAiEngine* m_pAiEngine;
    IStrategizer::IRtsGame* m_pGameModel;
};

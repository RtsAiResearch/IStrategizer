#pragma once

#include "ObjectFormatter.h"
#include "BWAPI.h"
#include "BWAPI\Client.h"

#include "IStrategizerEx.h"
#include "GameTraceCollector.h"

#define AIIDE_IO_DIR "bwapi-data\\write\\"

class YarmoukAIModule : public BWAPI::AIModule
{
public:
    YarmoukAIModule() :
        m_pIStrategizer(nullptr),
        m_pGameModel(nullptr),
        m_pTraceCollector(nullptr),
        m_isLearning(false)
    {}

    // Virtual functions for callbacks, leave these as they are.
    virtual void onStart();
    virtual void onEnd(bool isWinner);
    virtual void onFrame();
    virtual void onSendText(std::string text) {}
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
    // Everything below this line is safe to modify.

protected:
    void InitIStrategizer();
    void FinalizeIStrategizer();

    IStrategizer::IStrategizerEx* m_pIStrategizer;
    IStrategizer::RtsGame* m_pGameModel;
    IStrategizer::GameTraceCollector *m_pTraceCollector;
    bool m_isLearning;

};

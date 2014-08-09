#ifdef ISTRATEGIZER_EXPORTS
#define RTSAIENGINEAPI extern "C" __declspec(dllexport)
#endif

#include "IStrategizer.h"
#include "IStrategizerEx.h"
#include "SerializationEssentials.h"

using namespace IStrategizer;

class RtsAiEngineFactory : public IRtsAiEngineFactory
{
public:
    IRtsAiEngine* CreateEngine(_In_ const EngineParams& params, _In_ IRtsGame* pGame)
    {
        return new IStrategizerEx(params, pGame);
    }

    void DestroyEngine(_In_ IRtsAiEngine* pInstance)
    {
        delete pInstance;
    }
};

IStrategizer::IRtsAiEngineFactory* GetRtsAiEngineFactory()
{
    static RtsAiEngineFactory instance;
    return (IRtsAiEngineFactory*)&instance;
}

void RtsAiEngineSystemInit()
{
    SerializationEssentials::Init();
    IStrategizer::Init();
}

void RtsAiEngineSystemDeinit()
{
    RtsGame::FinalizeStaticData();
}

void RtsAiEngineExportGameStaticData()
{
    if (!g_Game)
        g_Game->ExportStaticData();
}
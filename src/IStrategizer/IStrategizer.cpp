#define RTSAIENGINEAPI extern "C" __declspec(dllexport)
#include "IStrategizer.h"
#include "IStrategizerEx.h"

using namespace IStrategizer;

class RtsAiEngineFactory
{
public:
    IRtsAiEngine* CreateEngine(_In_ const EngineParams& params, _In_ IRtsGame* pGame)
    {
        return new IStrategizerEx(params, (RtsGame*)pGame);
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


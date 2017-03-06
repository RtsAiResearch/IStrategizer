#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <BWAPI.h>

#include "YarmoukAIModule.h"

extern "C" __declspec(dllexport) void gameInit(BWAPI::Game* game) { BWAPI::BroodwarPtr = game; }
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        SetDllDirectoryA(ENGINE_IO_DIR);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) BWAPI::AIModule* newAIModule()
{
    return new YarmoukAIModule();
}

extern "C" __declspec(dllexport) void deleteAIModule(BWAPI::AIModule* pModule)
{
    delete pModule;
}

extern "C" __declspec(dllexport) void* getYarmoukEngine(BWAPI::AIModule* pModule)
{
    return ((YarmoukAIModule*)pModule)->Engine();
}


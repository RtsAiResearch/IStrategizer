#ifndef AIMODULELOADERDLL_H
#define AIMODULELOADERDLL_H

#ifndef LOADERAPI
#define LOADERAPI extern "C" __declspec(dllimport)
#endif // LOADERAPI

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

typedef void(*PfnMatchStartCallback)();
typedef void(*PfnMatchEndCallback)(bool);

LOADERAPI void AiModuleLoaderInit(const char* pBotDllFilename);
LOADERAPI bool AiModuleLoaderInitialized();
LOADERAPI void AiModuleLoaderShutdown();
LOADERAPI void AiModuleLoaderSetMatchStartCallback(PfnMatchStartCallback pfn);
LOADERAPI void AiModuleLoaderSetMatchEndCallback(PfnMatchEndCallback pfn);
LOADERAPI void* AIModuleLoaderGetEngine();

#endif // AIMODULELOADERDLL_H

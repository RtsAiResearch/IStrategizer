#ifndef ISTRATEGIZER_H
#define ISTRATEGIZER_H

#include "RtsAiEngine.h"

#ifdef ISTRATEGIZER_EXPORTS
#ifndef RTSAIENGINEAPI
#define RTSAIENGINEAPI extern "C" __declspec(dllimport)
#endif // RTSAIENGINEAPI
#else
#ifndef RTSAIENGINEAPI
#define RTSAIENGINEAPI 
#endif
#endif


RTSAIENGINEAPI IStrategizer::IRtsAiEngineFactory* GetRtsAiEngineFactory();
RTSAIENGINEAPI void RtsAiEngineSystemInit();
RTSAIENGINEAPI void RtsAiEngineSystemDeinit();
RTSAIENGINEAPI void RtsAiEngineExportGameStaticData();
RTSAIENGINEAPI const char** RtsAiEngineIdsName();

#endif // ISTRATEGIZER_H
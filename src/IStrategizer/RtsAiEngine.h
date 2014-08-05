#ifndef RTSAIENGINE_H
#define RTSAIENGINE_H

#include "MetaData.h"
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max

namespace IStrategizer
{
    struct EntityMessageData
    {
        EntityClassType EntityType;
        TID EntityId;
        PlayerType OwnerId;
        int X;
        int Y;
    };

    class IRtsGame
    {
    public:
        virtual ~IRtsGame() {}
    };

    struct EngineParams
    {
        unsigned GrndCtrlIMUpdateInterval;
        unsigned OccupanceIMUpdateInterval;
        int OccupanceIMCellSize;
        int GrndCtrlIMCellSize;
        PhaseType Phase;
        void* Consultant;
    };

    class IRtsAiEngine
    {
    public:
        virtual ~IRtsAiEngine() {}
        virtual void Update() = 0;
        virtual bool Init() = 0;
        virtual void SendEngineMessage(_In_ IStrategizer::MessageType msgTypeId) = 0;
        virtual void SendEngineEntityMessage(_In_ IStrategizer::MessageType msgTypeId, _In_ const IStrategizer::EntityMessageData& msgData) = 0;
        virtual void SetEngineReadWriteDir(_In_ const char* pReadPath, _In_ const char* pWritePath) = 0;
    };

    class IRtsAiEngineFactory
    {
    public:
        virtual ~IRtsAiEngineFactory() {}
        virtual IRtsAiEngine* CreateEngine(_In_ const EngineParams& params, _In_ IRtsGame* pGame) = 0;
        virtual void DestroyEngine(_In_ IRtsAiEngine* pInstance) = 0;
    };
}

#endif // RTSAIENGINE_H
#ifndef DATAMESSAGE_H
#define DATAMESSAGE_H

#include "Message.h"

#include "RtsAiEngine.h"
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#include <memory>

namespace IStrategizer
{
    // DataMessage that owns the lifetime of the data object
    template<class T>
    class DataMessage : public Message
    {
    public:
        DataMessage(unsigned gameFrame, MessageType msgType, T* pData) :
            Message(gameFrame, msgType),
            m_pData(pData)
        {}
        const T* Data() const { return &*m_pData; }
        T* Data() { return &*m_pData; }

    private:
        std::shared_ptr<T> m_pData;
    };

    // DataMessage that does not own the lifetime of the data object
    template<class T>
    class DataMessage<T*> : public Message
    {
    public:
        DataMessage(unsigned long gameFrame, MessageType msgType, T* pData) :
            Message(gameFrame, msgType),
            m_pData(pData)
        {}

        const T* Data() const { return m_pData; }
        T* Data() { return m_pData; }

    private:
        T* m_pData;
    };

    //----------------------------------------------------------------------------------------------
    typedef DataMessage<std::string> TextMessage;
    //----------------------------------------------------------------------------------------------
    typedef DataMessage<EntityMessageData> EntityCreateMessage;
    typedef DataMessage<EntityMessageData> EntityDestroyMessage;
    typedef DataMessage<EntityMessageData> EntityShowMessage;
    typedef DataMessage<EntityMessageData> EntityHideMessage;
    //----------------------------------------------------------------------------------------------
    struct GameEndMessageData
    {
        bool IsWinner;
        int Score;
        std::string MapName;
        std::string EnemyRace;
    };
    typedef DataMessage<GameEndMessageData> GameEndMessage;
    //----------------------------------------------------------------------------------------------
    struct EntityTrainedMessageData
    {
        TID TrainerId;
        TID EntityId;
        EntityClassType EntityType;
        EntityClassType TrainerType;

        EntityTrainedMessageData(TID p_trainerId, TID p_entityId, EntityClassType p_trainerType, EntityClassType p_entityType) :
            TrainerId(p_trainerId), EntityId(p_entityId), EntityType(p_trainerType), TrainerType(p_entityType) {}

        EntityTrainedMessageData() :
            TrainerId(0), EntityId(0), EntityType(ECLASS_END), TrainerType(ECLASS_END) {}
    };
    //----------------------------------------------------------------------------------------------
    struct BuildingBuiltMessageData
    {
        TID BuilderId;
        TID BuildingId;

        BuildingBuiltMessageData(TID p_builderId, TID p_buildingId) :
            BuilderId(p_builderId), BuildingId(p_buildingId) {}

        BuildingBuiltMessageData() :
            BuilderId(0), BuildingId(0) {}
    };
}

#endif // DATAMESSAGE_H

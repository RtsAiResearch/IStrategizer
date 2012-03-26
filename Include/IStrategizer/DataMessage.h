#ifndef DATAMESSAGE_H
#define DATAMESSAGE_H

#include "Message.h"

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif

template<class T>
class DataMessage : public Message
{
private:
    T* _data;
public:
    const T* Data();
    DataMessage(unsigned long p_gameCycle, MessageType p_messageTypeID, T* p_data);
    ~DataMessage();
};
//----------------------------------------------------------------------------------------------
template<class T>
DataMessage<T>::DataMessage(unsigned long p_gameCycle, MessageType p_messageTypeID, T* p_data) : Message(p_gameCycle, p_messageTypeID)
{
    _data = p_data;
}
//----------------------------------------------------------------------------------------------
template<class T>
const T* DataMessage<T>::Data()
{
    return _data;
}
//----------------------------------------------------------------------------------------------
template<class T>
DataMessage<T>::~DataMessage()
{
    delete _data;
}
//----------------------------------------------------------------------------------------------
typedef DataMessage<std::string> TextMessage;
//----------------------------------------------------------------------------------------------
struct EntityMessageData
{
	TID	EntityId;
	PlayerType OwnerId;
	int	X;
	int	Y;
};
//----------------------------------------------------------------------------------------------
typedef DataMessage<EntityMessageData> EntityCreateMessage;
typedef DataMessage<EntityMessageData> EntityDestroyMessage;
typedef DataMessage<EntityMessageData> EntityRenegadeMessage;
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

#endif // DATAMESSAGE_H

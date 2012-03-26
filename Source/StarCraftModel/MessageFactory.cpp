#include "MessageFactory.h"
#include "DataMessage.h"
#include "RawMessageData.h"
#include "Message.h"

#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif

Message* MessageFactory::GetMessage(MessageType p_message, void* p_rawMessageData)
{
	switch (p_message)
	{
	case MSG_Input:
		return CreateInputMessage(p_message, p_rawMessageData);
	case MSG_GameExit:
	case MSG_GameEnd:
	case MSG_EntityAttacked:
	case MSG_EntityTrained:
		return CreateEntityTrainedMessage(p_message, p_rawMessageData);
	case MSG_BuildingBuilt:
		return CreateBuildingBuiltMessage(p_message, p_rawMessageData);
	case MSG_EntityKilled:
		return CreateTrivialMessage(p_message, p_rawMessageData);
	}

	throw std::exception("MessageFactory: unsupported message");
}
//----------------------------------------------------------------------------------------------
Message* MessageFactory::CreateTrivialMessage(MessageType p_message, void* p_rawMessageData)
{
	unsigned long m_gameCycle = *(reinterpret_cast<unsigned long*>(p_rawMessageData));
	return new Message(m_gameCycle, p_message);
}
//----------------------------------------------------------------------------------------------
Message* MessageFactory::CreateInputMessage(MessageType p_message, void* p_rawMessageData)
{
	RawLogMessageData* m_rawMsgData = reinterpret_cast<RawLogMessageData*>(p_rawMessageData);
	return new DataMessage<const char>(m_rawMsgData->GameCycle, p_message, m_rawMsgData->Value.c_str());
}
//----------------------------------------------------------------------------------------------
Message* MessageFactory::CreateBuildingBuiltMessage(MessageType p_message, void* p_rawMessageData)
{
	RawBuildingBuiltMessageData* m_rawMsgData = reinterpret_cast<RawBuildingBuiltMessageData*>(p_rawMessageData);

	PlanStepParameters* m_params = new PlanStepParameters;
	m_params->operator [](PARAM_WorkerObjectId)		= m_rawMsgData->BuilderSlot;
	m_params->operator [](PARAM_BuildingObjectId)	= m_rawMsgData->BuildingSlot;
	//m_params->operator [](PARAM_WorkerClassId)		= g_DefinitionCrossMapping.EntityMapping.GetByFirst(m_rawMsgData->BuilderTypeIdent);
	//m_params->operator [](PARAM_BuildingClassId)	= g_DefinitionCrossMapping.EntityMapping.GetByFirst(m_rawMsgData->BuildingTypeIdent);

	// FIXME: Memory Leak
	return new DataMessage<PlanStepParameters>(m_rawMsgData->GameCycle, p_message, m_params);
}
//----------------------------------------------------------------------------------------------
Message* MessageFactory::CreateEntityTrainedMessage(MessageType p_message, void* p_rawMessageData)
{
	RawEntityTrainedMessageData* m_rawMsgData = reinterpret_cast<RawEntityTrainedMessageData*>(p_rawMessageData);

	PlanStepParameters* m_params = new PlanStepParameters;
	m_params->operator [](PARAM_TrainerObjectId)	= m_rawMsgData->TrainerSlot;
	m_params->operator [](PARAM_EntityObjectId)		= m_rawMsgData->EntitySlot;
	//m_params->operator [](PARAM_TrainerObjectId)	= g_DefinitionCrossMapping.EntityMapping.GetByFirst(m_rawMsgData->TrainerTypeIdent);
	//m_params->operator [](PARAM_EntityClassId)		= g_DefinitionCrossMapping.EntityMapping.GetByFirst(m_rawMsgData->EntityTypeIdent);

	// FIXME: Memory Leak
	return new DataMessage<PlanStepParameters>(m_rawMsgData->GameCycle, p_message, m_params);
}

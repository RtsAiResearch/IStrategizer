#ifndef MESSAGEFACTORY_H
#define MESSAGEFACTORY_H

struct RawLogMessageData;
class Message;
class GoalEx;
class GameStateEx;

namespace MetaData
{
	enum MessageType;
}

class MessageFactory
{
public:
	inline static MessageFactory&	Instance() { static MessageFactory m_instance; return m_instance; };
	Message*						GetMessage(MetaData::MessageType p_message, void* p_rawMessageData);

#define g_MessageFactory MessageFactory::Instance()

private:
	MessageFactory () {};
	Message*    CreateInputMessage(MetaData::MessageType p_message, void* p_rawMessageData);
	Message*	CreateTrivialMessage(MetaData::MessageType p_message, void* p_rawMessageData);
	Message*	CreateBuildingBuiltMessage(MetaData::MessageType p_message, void* p_rawMessageData);
	Message*	CreateEntityTrainedMessage(MetaData::MessageType p_message, void* p_rawMessageData);
};

#endif // MESSAGEFACTORY_H

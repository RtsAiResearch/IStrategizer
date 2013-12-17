#ifndef MESSAGEFACTORY_H
#define MESSAGEFACTORY_H

struct RawLogMessageData;
class Message;
class GoalEx;
class GameStateEx;

namespace IStrategizer
{
	enum MessageType;
}

class MessageFactory
{
public:
	inline static MessageFactory&	Instance() { static MessageFactory m_instance; return m_instance; };
	Message*						GetMessage(IStrategizer::MessageType p_message, void* p_rawMessageData);

#define g_MessageFactory MessageFactory::Instance()

private:
	MessageFactory () {};
	Message*    CreateInputMessage(IStrategizer::MessageType p_message, void* p_rawMessageData);
	Message*	CreateTrivialMessage(IStrategizer::MessageType p_message, void* p_rawMessageData);
	Message*	CreateBuildingBuiltMessage(IStrategizer::MessageType p_message, void* p_rawMessageData);
	Message*	CreateEntityTrainedMessage(IStrategizer::MessageType p_message, void* p_rawMessageData);
};

#endif // MESSAGEFACTORY_H

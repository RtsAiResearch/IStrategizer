#ifndef MESSAGEFACTORY_H
#define MESSAGEFACTORY_H

namespace IStrategizer
{
    enum MessageType;
    struct RawLogMessageData;
    class Message;
    class GoalEx;
    class GameStateEx;

    class MessageFactory
    {
    public:
        inline static MessageFactory& Instance() { static MessageFactory m_instance; return m_instance; };
        Message* GetMessage(MessageType p_message, void* p_rawMessageData);

#define g_MessageFactory MessageFactory::Instance()

    private:
        MessageFactory () {};
        Message*    CreateInputMessage(MessageType p_message, void* p_rawMessageData);
        Message* CreateTrivialMessage(MessageType p_message, void* p_rawMessageData);
        Message* CreateBuildingBuiltMessage(MessageType p_message, void* p_rawMessageData);
        Message* CreateEntityTrainedMessage(MessageType p_message, void* p_rawMessageData);
    };
}

#endif // MESSAGEFACTORY_H

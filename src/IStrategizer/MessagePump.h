#ifndef MESSAGEPUMP_H
#define MESSAGEPUMP_H

#include "MessagePumpSubject.h"
#include <queue>

namespace IStrategizer
{
    class Message;

    typedef std::queue<Message*> MessageQueue;

    class MessagePump : public MessagePumpSubject
    {
    private:
        MessageQueue _messageQueue;
    public:
        static MessagePump* Instance();
        void Update(int gameFrame);
        void Send(Message* p_message, bool p_immediate = false);

    protected:
        MessagePump();
        void DeliverMessage(Message* p_message);
        ~MessagePump();
    };

#define g_MessagePump IStrategizer::MessagePump::Instance()
}

#endif // MESSAGEPUMP_H

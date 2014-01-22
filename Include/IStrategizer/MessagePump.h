#ifndef MESSAGEPUMP_H
#define MESSAGEPUMP_H

#include <queue>
#include "MessagePumpSubject.h"
#include "WorldClock.h"

namespace IStrategizer
{
    class Message;

    typedef std::queue<Message*> MessageQueue;

    class MessagePump : public MessagePumpSubject
    {
    private:
        MessageQueue _messageQueue;
        RtsGame& _rtsGame;
    public:
        static MessagePump& Instance(RtsGame& p_RtsGame);
        void Update(const WorldClock& p_clock);
        void Send(Message* p_message, bool p_immediate = false);
    protected:
        MessagePump(RtsGame& p_RtsGame);
        void DeliverMessage(Message* p_message);
        ~MessagePump();
    };
}

#endif // MESSAGEPUMP_H

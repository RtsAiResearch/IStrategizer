#ifndef MESSAGEPUMP_H
#define MESSAGEPUMP_H

#include "MessagePumpSubject.h"
#include <queue>

class Message;

typedef std::queue<Message*> MessageQueue;

class MessagePump : public MessagePumpSubject
{
private:
    MessageQueue _messageQueue;
public:
    static MessagePump& Instance();
    void Update(unsigned long p_gameCycle);
    void Send(Message* p_message, bool p_immediate = false);
protected:
    MessagePump();
    void DeliverMessage(Message* p_message);
    ~MessagePump();
};


#define g_MessagePump MessagePump::Instance() 

#endif // MESSAGEPUMP_H

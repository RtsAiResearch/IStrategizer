#ifndef MESSAGEPUMPSUBJECT_H
#define MESSAGEPUMPSUBJECT_H

#include <map>
#include <list>

#include "Message.h"
#include "MessagePumpObserver.h"

namespace IStrategizer
{
    enum MessageType;

    typedef std::list<MessagePumpObserver*> ObserverList;
    typedef std::map<MessageType, ObserverList* > MessageObserversTable;

    class MessagePumpSubject
    {
    protected:
        MessageObserversTable _messageObserversTable;

    public:
        void RegisterForMessage(RtsGame& p_RtsGame, MessageType p_messageTypeID, MessagePumpObserver* p_observer);
        void UnRegisterForMessage(RtsGame& p_RtsGame, MessageType p_messageTypeID, MessagePumpObserver* p_observer);
        void UnregisterForAllMessages(RtsGame& p_RtsGame, MessagePumpObserver* p_observer);
        void AddMessage(RtsGame& p_RtsGame, MessageType p_messageTypeID);
        void OnMessageSent(RtsGame& p_RtsGame, Message* p_message);
        virtual ~MessagePumpSubject();
    };
}

#endif // MESSAGEPUMPSUBJECT_H


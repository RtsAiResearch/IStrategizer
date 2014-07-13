#ifndef MESSAGEPUMPSUBJECT_H
#define MESSAGEPUMPSUBJECT_H

#include <map>
#include <list>

#include "Message.h"
#include "IMessagePumpObserver.h"

namespace IStrategizer
{
    enum MessageType;

    typedef std::list<IMessagePumpObserver*> ObserverList;
    typedef std::map<MessageType, ObserverList* > MessageObserversTable;

    class MessagePumpSubject
    {
    protected:
        MessageObserversTable _messageObserversTable;

    public:
        void RegisterForMessage(MessageType p_messageTypeID, IMessagePumpObserver* p_observer);
        void UnRegisterForMessage(MessageType p_messageTypeID, IMessagePumpObserver* p_observer);
        void UnregisterForAllMessages(IMessagePumpObserver* p_observer);
        void AddMessage(MessageType p_messageTypeID);
        void OnMessageSent(Message* p_message);
        void UnregisterAllObservers() { for (auto msgEntry : _messageObserversTable) msgEntry.second->clear(); }
        virtual ~MessagePumpSubject();
    };
}

#endif // MESSAGEPUMPSUBJECT_H


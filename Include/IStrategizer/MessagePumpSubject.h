#ifndef MESSAGEPUMPSUBJECT_H
#define MESSAGEPUMPSUBJECT_H

#include <map>
#include <list>

#include "Message.h"
#include "MessagePumpObserver.h"

namespace IStrategizer
{
	enum MessageType;
}

typedef std::list<MessagePumpObserver*> ObserverList;
typedef std::map<IStrategizer::MessageType, ObserverList* > MessageObserversTable;

class MessagePumpSubject
{
protected:
    MessageObserversTable _messageObserversTable;

public:
    void RegisterForMessage(IStrategizer::MessageType p_messageTypeID, MessagePumpObserver* p_observer);
    void UnRegisterForMessage(IStrategizer::MessageType p_messageTypeID, MessagePumpObserver* p_observer);
	void UnregisterForAllMessages(MessagePumpObserver* p_observer);
    void AddMessage(IStrategizer::MessageType p_messageTypeID);
    void OnMessageSent(Message* p_message);
    virtual ~MessagePumpSubject();
};

#endif // MESSAGEPUMPSUBJECT_H


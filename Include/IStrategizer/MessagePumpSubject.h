#ifndef MESSAGEPUMPSUBJECT_H
#define MESSAGEPUMPSUBJECT_H

#include <map>
#include <list>

#include "Message.h"
#include "MessagePumpObserver.h"

namespace MetaData
{
	enum MessageType;
}

typedef std::list<MessagePumpObserver*> ObserverList;
typedef std::map<MetaData::MessageType, ObserverList* > MessageObserversTable;

class MessagePumpSubject
{
protected:
    MessageObserversTable _messageObserversTable;

public:
    void RegisterForMessage(MetaData::MessageType p_messageTypeID, MessagePumpObserver* p_observer);
    void UnRegisterForMessage(MetaData::MessageType p_messageTypeID, MessagePumpObserver* p_observer);
	void UnregisterForAllMessages(MessagePumpObserver* p_observer);
    void AddMessage(MetaData::MessageType p_messageTypeID);
    void OnMessageSent(Message* p_message);
    ~MessagePumpSubject();
};

#endif // MESSAGEPUMPSUBJECT_H


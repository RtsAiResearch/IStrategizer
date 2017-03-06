#include "MessagePumpSubject.h"
#include "MetaData.h"
#include "Logger.h"
#include <algorithm>

using namespace IStrategizer;

void MessagePumpSubject::AddMessage(MessageType p_messageTypeID)
{
    if(_messageObserversTable.find(p_messageTypeID) == _messageObserversTable.end())
    {
        _messageObserversTable.insert(std::pair<MessageType, ObserverList* >(p_messageTypeID, new ObserverList));
    }
}
//----------------------------------------------------------------------------------------------
void MessagePumpSubject::RegisterForMessage(MessageType p_messageTypeID, IMessagePumpObserver* p_observer)
{
    ObserverList* m_observerList;

    if(_messageObserversTable.find(p_messageTypeID) != _messageObserversTable.end())
    {
        m_observerList = _messageObserversTable[p_messageTypeID];
        if(find(m_observerList->begin(), m_observerList->end(), p_observer) == m_observerList->end())
        {
            m_observerList->push_back(p_observer);
        }
    }
}
//----------------------------------------------------------------------------------------------
void MessagePumpSubject::UnRegisterForMessage(MessageType p_messageTypeID, IMessagePumpObserver* p_observer)
{
    if(_messageObserversTable.find(p_messageTypeID) != _messageObserversTable.end())
    {
        _messageObserversTable[p_messageTypeID]->remove(p_observer);
    }
}
//////////////////////////////////////////////////////////////////////////
void MessagePumpSubject::UnregisterForAllMessages(IMessagePumpObserver* p_observer)
{
    ObserverList* m_observerList;

    for(MessageObserversTable::iterator itr = _messageObserversTable.begin();
        itr != _messageObserversTable.end(); ++itr)
    {
        m_observerList = itr->second;
        m_observerList->remove(p_observer);
    }
}
//----------------------------------------------------------------------------------------------
void MessagePumpSubject::OnMessageSent(Message* p_message)
{
    ObserverList* m_observerList;

	LogInfo("Delivering message %s", p_message->ToString());

    if(_messageObserversTable.find((MessageType)p_message->TypeId()) != _messageObserversTable.end())
    {
        m_observerList = _messageObserversTable[(MessageType)p_message->TypeId()];

        for(ObserverList::iterator itr = m_observerList->begin();
            itr != m_observerList->end();
            itr++)
        {
            (*itr)->NotifyMessegeSent(p_message);
        }
    }
}

MessagePumpSubject::~MessagePumpSubject()
{
    for(MessageObserversTable::iterator itr = _messageObserversTable.begin();
        itr != _messageObserversTable.end();
        itr++)
    {
        delete itr->second;
    }
}

#include "MessagePump.h"
#include "Persist.h"

#ifndef METADATA_H
#include "MetaData.h"
#endif
using namespace IStrategizer;

MessagePump::MessagePump()
{
    AddMessage(MSG_Input);
    AddMessage(MSG_EntityCreate);
    AddMessage(MSG_EntityDestroy);
    AddMessage(MSG_EntityRenegade);
    AddMessage(MSG_GameStart);
    AddMessage(MSG_GameEnd);
    AddMessage(MSG_PlanStructureChange);

    // Obsolete Messages
    AddMessage(MSG_GameActionLog);
    AddMessage(MSG_GameExit);
    AddMessage(MSG_EntityAttacked);
    AddMessage(MSG_EntityKilled);
    AddMessage(MSG_EntityTrained);
    AddMessage(MSG_BuildingBuilt);
}
//----------------------------------------------------------------------------------------------
MessagePump& MessagePump::Instance()
{
    static MessagePump m_instance;
    return m_instance;
}
//----------------------------------------------------------------------------------------------
void MessagePump::Send(Message* p_message, bool p_immediate)
{
    if(p_immediate)
    {
        DeliverMessage(p_message);
    }
    else
    {
        _messageQueue.push(p_message);
    }
}
//----------------------------------------------------------------------------------------------
void MessagePump::DeliverMessage(Message* p_message)
{
    p_message->Delivered(true);

    OnMessageSent(p_message);

    delete p_message;
}
//----------------------------------------------------------------------------------------------
void MessagePump::Update(const WorldClock& p_clock)
{
    Message* m_message;

    while(!_messageQueue.empty())
    {
        m_message = _messageQueue.front();
        _messageQueue.pop();

        DeliverMessage(m_message);
    }
}
//----------------------------------------------------------------------------------------------
MessagePump::~MessagePump()
{
    Message* m_message;

    while(!_messageQueue.empty())
    {
        m_message = _messageQueue.front();
        _messageQueue.pop();

        delete m_message;
    }
}

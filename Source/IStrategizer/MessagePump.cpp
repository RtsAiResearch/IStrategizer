#include "MessagePump.h"
#include "Persist.h"

#ifndef METADATA_H
#include "MetaData.h"
#endif
using namespace IStrategizer;

MessagePump::MessagePump(RtsGame& p_RtsGame)
    : _rtsGame(p_RtsGame)
{
    AddMessage(_rtsGame, MSG_Input);
    AddMessage(_rtsGame, MSG_EntityCreate);
    AddMessage(_rtsGame, MSG_EntityDestroy);
    AddMessage(_rtsGame, MSG_EntityRenegade);
    AddMessage(_rtsGame, MSp_RtsGameStart);
    AddMessage(_rtsGame, MSp_RtsGameEnd);
    AddMessage(_rtsGame, MSG_PlanStructureChange);
                
    // Obsolete Messages
    AddMessage(_rtsGame, MSp_RtsGameActionLog);
    AddMessage(_rtsGame, MSp_RtsGameExit);
    AddMessage(_rtsGame, MSG_EntityAttacked);
    AddMessage(_rtsGame, MSG_EntityKilled);
    AddMessage(_rtsGame, MSG_EntityTrained);
    AddMessage(_rtsGame, MSG_BuildingBuilt);
}
//----------------------------------------------------------------------------------------------
MessagePump& MessagePump::Instance(RtsGame& p_RtsGame)
{
    static MessagePump m_instance (p_RtsGame);
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

    OnMessageSent(_rtsGame, p_message);

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

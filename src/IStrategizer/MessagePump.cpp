#include "MessagePump.h"
#include "MetaData.h"
#include "Logger.h"

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
    AddMessage(MSG_PlanComplete);
    AddMessage(MSG_BattleComplete);
    AddMessage(MSG_GameActionLog);
}
//----------------------------------------------------------------------------------------------
MessagePump* MessagePump::Instance()
{
    static MessagePump* m_instance = nullptr;

	if (nullptr == m_instance)
	{
		m_instance = new MessagePump;
	}

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
		LogDebugInfo("Queuing message %s", p_message->ToString());
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

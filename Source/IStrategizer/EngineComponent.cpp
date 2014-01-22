#include <string>
#include <cassert>
#include "EngineComponent.h"
#include "MessagePump.h"
#include "DataMessage.h"

using namespace IStrategizer;

EngineComponent::EngineComponent(RtsGame& p_RtsGame, const char* p_name)
{
    m_pName = p_name;
    LogInfo("%s is initializing ...", m_pName);
    MessagePump::Instance(p_RtsGame).RegisterForMessage(p_RtsGame, MSG_Input, this);
}
//----------------------------------------------------------------------------------------------
void EngineComponent::NotifyMessegeSent(RtsGame& p_RtsGame, Message *p_message)
{
    assert(p_message->MessageTypeID() == MSG_Input);
    DataMessage<string>* inputMessage = static_cast<DataMessage<string>*>(p_message);

    if(!strncmp(inputMessage->Data()->c_str(), m_pName, strlen(m_pName)))
    {
        ExecuteCommand(inputMessage->Data()->c_str() + strlen(m_pName) + 1);
    }
}
//----------------------------------------------------------------------------------------------
EngineComponent::~EngineComponent()
{
    LogInfo("%s is finalizing ...", m_pName);
}

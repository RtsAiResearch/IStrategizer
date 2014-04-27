#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif
#ifndef MESSAGEPUMP_H
#include "MessagePump.h"
#endif
#ifndef DATAMESSAGE_H
#include "DataMessage.h"
#endif

#include <string>
#include <cassert>

using namespace IStrategizer;
using namespace std;

EngineComponent::EngineComponent(const char* p_pName)
    : m_pName(p_pName)
{
    LogInfo("%s is initializing ...", m_pName);
    g_MessagePump.RegisterForMessage(MSG_Input, this);
}
//----------------------------------------------------------------------------------------------
void EngineComponent::NotifyMessegeSent(Message *p_message)
{
    _ASSERTE(p_message->MessageTypeID() == MSG_Input);
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

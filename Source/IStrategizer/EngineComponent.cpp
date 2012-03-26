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

EngineComponent::EngineComponent(const char* p_name)  : TraceableComponent(p_name)
{
    g_MessagePump.RegisterForMessage(MSG_Input, this);
    Log(LOG_Information, "initializing...");
}
//----------------------------------------------------------------------------------------------
void EngineComponent::NotifyMessegeSent(Message *p_message)
{
    assert(p_message->MessageTypeID() == MSG_Input);
    DataMessage<string>* inputMessage = static_cast<DataMessage<string>*>(p_message);

    if(!strncmp(inputMessage->Data()->c_str(), _name, strlen(_name)))
    {
        ExecuteCommand(inputMessage->Data()->c_str() + strlen(_name) + 1);
    }
}
//----------------------------------------------------------------------------------------------
EngineComponent::~EngineComponent()
{
    Log(LOG_Information, "finalizing...");
    WriteLog(_name);
}
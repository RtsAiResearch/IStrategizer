#include "Message.h"

#ifndef METADATA_H
#include "MetaData.h"
#endif
using namespace MetaData;

Message::Message(unsigned long p_gameCycle, MessageType p_messageTypeID)
{
    _gameCycle      = p_gameCycle;
    _messageTypeID  = p_messageTypeID;
    _delivered      = false;
    _priority       = 1;
}
//----------------------------------------------------------------------------------------------
unsigned long Message::GameCycle()
{
    return _gameCycle;
}
//----------------------------------------------------------------------------------------------
int Message::MessageTypeID()
{
    return _messageTypeID;
}
//----------------------------------------------------------------------------------------------
bool  Message::Delivered()
{
    return _delivered;
}
//----------------------------------------------------------------------------------------------
void Message::Delivered(bool p_delivered)
{
    _delivered = p_delivered;
}
//----------------------------------------------------------------------------------------------
int Message::Priority()
{
    return _priority;
}
//----------------------------------------------------------------------------------------------
Message::~Message()
{

}
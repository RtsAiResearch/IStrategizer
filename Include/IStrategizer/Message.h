#ifndef MESSAGE_H
#define MESSAGE_H

namespace MetaData
{
	enum MessageType;
}

class Message
{
private:
	MetaData::MessageType _messageTypeID;
    unsigned long				_gameCycle; 
    bool						_delivered;
    int							_priority;
public:
    unsigned long GameCycle();
    int MessageTypeID();
    bool Delivered();
    void Delivered(bool p_delivered);
    int Priority();
    Message(unsigned long p_gameCycle, MetaData::MessageType p_messageTypeID);
	virtual ~Message();
};

#endif // MESSAGE_H

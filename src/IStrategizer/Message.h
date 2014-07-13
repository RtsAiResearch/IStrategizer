#ifndef MESSAGE_H
#define MESSAGE_H

#include "TypesMetaData.h"

namespace IStrategizer
{
    class Message
    {
    public:
		Message(unsigned long p_gameCycle, MessageType p_messageTypeID);
		virtual ~Message();
        unsigned long GameCycle();
        int MessageTypeID();
        bool Delivered();
        void Delivered(bool p_delivered);
        int Priority();
		const char* ToString()  const { return Enums[_messageTypeID]; }

	private:
		MessageType _messageTypeID;
		unsigned long _gameCycle;
		bool _delivered;
		int _priority;
    };


}

#endif // MESSAGE_H

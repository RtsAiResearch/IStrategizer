#ifndef MESSAGEPUMPOBSERVER_H
#define MESSAGEPUMPOBSERVER_H

namespace IStrategizer
{
    class Message;
    class IMessagePumpObserver
    {
    public:
        virtual void NotifyMessegeSent(Message* p_pMessage) = 0;
		virtual ~IMessagePumpObserver() {}
    };
}

#endif // MESSAGEPUMPOBSERVER_H


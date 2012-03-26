#ifndef MESSAGEPUMPOBSERVER_H
#define MESSAGEPUMPOBSERVER_H

class Message;
class MessagePumpObserver
{
public:
    virtual void NotifyMessegeSent(Message* p_pMessage) = 0;
	virtual ~MessagePumpObserver();
};

#endif // MESSAGEPUMPOBSERVER_H


#ifndef MESSAGEPUMPOBSERVER_H
#define MESSAGEPUMPOBSERVER_H

#include <vector>

using namespace std;

namespace IStrategizer
{
    class Message;
    class RtsGame;

    class MessagePumpObserver
    {
    public:
        virtual void NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_Message) = 0;
        virtual ~MessagePumpObserver();
    };
}

#endif // MESSAGEPUMPOBSERVER_H


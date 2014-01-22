#ifndef ENGINECOMPONENT_H
#define ENGINECOMPONENT_H

#include "Logger.h"
#include "MessagePumpObserver.h"
#include "Message.h"

namespace IStrategizer
{
    class RtsGame;

    class EngineComponent : public MessagePumpObserver
    {
    public:
        EngineComponent(RtsGame& p_RtsGame, const char* p_name);
        ~EngineComponent();
        void NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_message);
        const char* Name() const { return m_pName; }

    protected:
        virtual void ExecuteCommand(const char* p_cmd) { LogError("Command '%s' not supported", p_cmd); }

    private:
        const char* m_pName;
    };
}
#endif // ENGINECOMPONENT_H

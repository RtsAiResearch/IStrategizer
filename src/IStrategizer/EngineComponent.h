#ifndef ENGINECOMPONENT_H
#define ENGINECOMPONENT_H

#ifndef LOGGER_H
#include "Logger.h"
#endif
#ifndef MESSAGEPUMPOBSERVER_H
#include "MessagePumpObserver.h"
#endif

namespace IStrategizer
{
    class EngineComponent : public MessagePumpObserver
    {
    public:
        EngineComponent(const char* p_name);
        ~EngineComponent();
        void NotifyMessegeSent(Message* p_message);
        const char* Name() const { return m_pName; }

    protected:
        virtual void ExecuteCommand(const char* p_cmd) { LogError("Command '%s' not supported", p_cmd); }

    private:
        const char* m_pName;
    };
}
#endif // ENGINECOMPONENT_H

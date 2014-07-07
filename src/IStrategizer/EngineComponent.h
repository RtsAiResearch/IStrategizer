#ifndef ENGINECOMPONENT_H
#define ENGINECOMPONENT_H

#include "Logger.h"
#include "MessagePumpObserver.h"
#include "EngineObject.h"

namespace IStrategizer
{
    class EngineComponent : public EngineObject, public MessagePumpObserver
    {
    public:
        EngineComponent(const char* p_name);
        ~EngineComponent();
        void NotifyMessegeSent(Message* p_message);
        const char* Name() const { return m_pName; }
        virtual std::string ToString(bool minimal) const { return std::string(m_pName); }

    protected:
        virtual void ExecuteCommand(const char* p_cmd) { LogError("Command '%s' not supported", p_cmd); }

    private:
        const char* m_pName;
    };
}
#endif // ENGINECOMPONENT_H

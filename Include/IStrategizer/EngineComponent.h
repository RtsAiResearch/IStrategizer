#ifndef ENGINECOMPONENT_H
#define ENGINECOMPONENT_H

#ifndef TRACEABLECOMPONENT_H
#include "TraceableComponent.h"
#endif
#ifndef MESSAGEPUMPOBSERVER_H
#include "MessagePumpObserver.h"
#endif

#include <string>
using namespace std;

class EngineComponent : public TraceableComponent, public MessagePumpObserver
{
protected:

    virtual void ExecuteCommand(const char* p_cmd) = 0;
public:
            EngineComponent(const char* p_name);
    void    NotifyMessegeSent(Message* p_message);
            ~EngineComponent();
};

#endif // ENGINECOMPONENT_H
#include "MessagePumpObserver.h"
#include "MessagePump.h"

using namespace IStrategizer;

MessagePumpObserver::~MessagePumpObserver()
{
    g_MessagePump.UnregisterForAllMessages(this);
}

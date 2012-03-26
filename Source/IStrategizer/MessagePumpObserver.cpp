#include "MessagePumpObserver.h"
#include "MessagePump.h"

MessagePumpObserver::~MessagePumpObserver()
{
	g_MessagePump.UnregisterForAllMessages(this);
}
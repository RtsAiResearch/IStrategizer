///> [Serializable]
#ifndef EVENTTRIGGERED_H
#define EVENTTRIGGERED_H

#ifndef CONDITIONEX_H
#include "ConditionEx.h"
#endif
#ifndef MESSAGEPUMPOBSERVER_H
#include "MessagePumpObserver.h"
#endif

namespace MetaData
{
	enum MessageType;
}
class IComparable;

///> class=EventTriggered
///> parent=ConditionEx
class EventTriggered : public ConditionEx, public MessagePumpObserver
{
	bool		_triggered;
public:
	EventTriggered() : _triggered(false) {}
	EventTriggered(MetaData::MessageType p_eventTypeId, const PlanStepParameters& p_eventArgs);
	bool	    Evaluate();
	void        Copy(IClonable* p_dest);
	bool		Consume(int p_amount) { return true; }
	void		NotifyMessegeSent(Message* p_message);
	~EventTriggered();
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "EventTriggered"; }
	int         TypeSize()  { return sizeof(EventTriggered); }
	UserObject* Prototype() { return new EventTriggered; }	
	//----------------------------------------------------------------------------------------------
};
#endif	// EVENTTRIGGERED_H
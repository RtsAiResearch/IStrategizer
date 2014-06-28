///> [Serializable]
#ifndef EVENTTRIGGERED_H
#define EVENTTRIGGERED_H

#ifndef CONDITIONEX_H
#include "ConditionEx.h"
#endif
#ifndef MESSAGEPUMPOBSERVER_H
#include "MessagePumpObserver.h"
#endif

namespace IStrategizer
{
    enum MessageType;
    class IComparable;

    ///> class=EventTriggered
    ///> parent=ConditionEx
    class EventTriggered : public ConditionEx, public MessagePumpObserver
    {
        OBJECT_SERIALIZABLE(EventTriggered);

    private:
        bool _triggered;

    public:
        EventTriggered() : _triggered(false) {}
        EventTriggered(IStrategizer::MessageType p_eventTypeId, const PlanStepParameters& p_eventArgs);
        bool     Evaluate();
        void        Copy(IClonable* p_dest);
        bool Consume(int p_amount) { return true; }
        void NotifyMessegeSent(Message* p_message);
        ~EventTriggered();
    };
}

#endif // EVENTTRIGGERED_H

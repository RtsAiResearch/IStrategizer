#ifndef EVENTTRIGGERED_H
#include "EventTriggered.h"
#endif

#ifndef METADATA_H
#include "MetaData.h"
#endif
#ifndef DATAMESSAGE_H
#include "DataMessage.h"
#endif
#ifndef MESSAGEPUMP_H
#include "MessagePump.h"
#endif
using namespace IStrategizer;

EventTriggered::EventTriggered(MessageType p_eventTypeId, const PlanStepParameters& p_eventArgs) : _triggered(false)
{
    g_MessagePump->RegisterForMessage(p_eventTypeId, this);

    m_params = p_eventArgs;
    m_params[PARAM_MessageTypeId] = (int)p_eventTypeId;
}
//----------------------------------------------------------------------------------------------
void EventTriggered::NotifyMessegeSent(Message* p_message)
{
    if (_triggered)
        return;

    if (p_message->TypeId() == m_params[PARAM_MessageTypeId])
    {
        DataMessage<PlanStepParameters>* m_concMsg = static_cast<DataMessage<PlanStepParameters>*>(p_message);
        const PlanStepParameters* params = m_concMsg->Data();

        // excluding message type id from params
        //assert(m_params->size() == _conditionParameters.size() - 1);

        _triggered = true;
        for (PlanStepParameters::const_iterator itr = params->begin();
            itr != params->end();
            ++itr)
        {
            if (m_params.find(itr->first) != m_params.end())
            {
                _triggered &= (itr->second == m_params[itr->first]);

                if (!_triggered)
                    break;
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
bool EventTriggered::Evaluate()
{
    return _triggered;
}
//----------------------------------------------------------------------------------------------
void EventTriggered::Copy(IClonable* p_dest)
{
    ConditionEx::Copy(p_dest);

    EventTriggered* m_dest = static_cast<EventTriggered*>(p_dest);
    m_dest->_triggered = _triggered;
}
//----------------------------------------------------------------------------------------------
EventTriggered::~EventTriggered()
{
    g_MessagePump->UnRegisterForMessage((MessageType)m_params[PARAM_MessageTypeId], this);
}

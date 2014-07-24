#ifndef FSMMACHINE_H
#define FSMMACHINE_H

#include "FSMState.h"
#include <map>

namespace IStrategizer
{
    class StackFSM : public FSMState
    {
    public:
        StackFSM(FSMStateTypeID initialState, FSMStateTypeID finalState, FSMStateTypeID typeId, EngineObject* pController) :
            FSMState(typeId, pController),
            m_initialState(initialState),
            m_finalState(finalState),
            m_firstUpdate(true) 
        {
            m_states.push(m_initialState);
        }

        virtual void CheckTransitions() = 0;

        void Reset()
        {
            while (!m_states.empty())
                m_states.pop();

            m_states.push(m_initialState);
        }

        bool IsInFinalState() const 
        {
            // Our Stack FSM has the property of never being empty
            // There should be always at least one state in the stack
            // e.g the Idle state can be the always state in the bottom
            _ASSERTE(m_states.empty());
            return m_states.top() == m_finalState; 
        }

        void Update()
        {
            // Don't do anything if you have no states
            if(m_states.empty())
                return;

            // Update current state, and check for a transition
            auto pCurrentState = CurrentState();
            
            // Call enter for the first state 
            if (m_firstUpdate)
            {
                m_firstUpdate = false;
                pCurrentState->Enter();
            }

            pCurrentState->Update();

            CheckTransitions();
        }
        
    protected:
        void AddState(std::shared_ptr<FSMState> state) { m_stateMap[state->TypeId()] = state; }
        void PushState(FSMStateTypeID newState) { m_states.push(newState); CurrentState()->Enter(); }
        void PopState() { CurrentState()->Exit(); m_states.pop(); }
        FSMStatePtr CurrentState() { _ASSERTE(!m_states.empty()); return m_stateMap.at(m_states.top()); }

    private:
        std::map<FSMStateTypeID, FSMStatePtr> m_stateMap;
        std::stack<FSMStateTypeID> m_states;
        FSMStateTypeID m_initialState;
        FSMStateTypeID m_finalState;
        bool m_firstUpdate;
    };

    typedef std::shared_ptr<StackFSM> StackFSMPtr;
}

#endif // FSMMACHINE_H
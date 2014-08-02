#ifndef FSMMACHINE_H
#define FSMMACHINE_H

#include "FSMState.h"
#include <map>

namespace IStrategizer
{
    class StackFSM : public FSMState
    {
    public:
        StackFSM(const char* pName, FSMStateTypeID initialState, FSMStateTypeID finalState, FSMStateTypeID typeId, EngineObject* pController) :
            FSMState(typeId, pName, pController),
            m_initialState(initialState),
            m_finalState(finalState),
            m_firstUpdate(true) 
        {
            m_states.push(m_initialState);
        }

        void SetController(_In_ EngineObject* pController) { m_pController = pController; }

        virtual void CheckTransitions() = 0;

        virtual void Reset()
        {
            while (!m_states.empty())
                PopState();

            m_states.push(m_initialState);
            m_firstUpdate = true;
        }

        bool IsInFinalState() const 
        {
            // Our Stack FSM has the property of never being empty
            // There should be always at least one state in the stack
            // e.g the Idle state can be the always state in the bottom
            if (m_states.empty())
                return false;
            else
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

        FSMStatePtr CurrentState() { _ASSERTE(!m_states.empty()); return m_stateMap.at(m_states.top()); }
        
    protected:
        void AddState(_In_ std::shared_ptr<FSMState> state) { m_stateMap[state->TypeId()] = state; state->Parent(this); }
        void PushState(_In_ FSMStateTypeID newState) 
        {
            if (!m_states.empty())
            {
                auto oldState = CurrentState();
                m_states.push(newState); CurrentState()->Enter();
                LogInfo("%s: %s -> %s", ToString().c_str(), oldState->ToString(true).c_str(), CurrentState()->ToString(true).c_str());
            }
            else
            {
                m_states.push(newState); 
                CurrentState()->Enter();
                LogInfo("%s: (NULL) -> %s", ToString().c_str(), CurrentState()->ToString(true).c_str());
            }
        }

        void PopState()
        {
            auto oldState = CurrentState();
            CurrentState()->Exit(); 
            m_states.pop();

            if (!m_states.empty())
            {
                LogInfo("%s: %s -> %s", ToString().c_str(), oldState->ToString(true).c_str(), CurrentState()->ToString(true).c_str());
            }
            else
            {
                LogInfo("%s: %s -> (NULL)", ToString().c_str(), oldState->ToString(true).c_str());
            }

        }

        void PopAllAndPushState(_In_ FSMStateTypeID newState)
        {
            while (!m_states.empty())
                PopState();
            PushState(newState);
        }


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
#ifndef FSMMACHINE_H
#define FSMMACHINE_H

#include "FSMState.h"
#include <map>

namespace IStrategizer
{
    class StackFSM : public FSMState
    {
    public:
        StackFSM(FSMStateTypeID defaultState, EngineObject* pController) :
            FSMState(pController),
            m_defaultState(defaultState),
            m_firstUpdate(true) 
        {}

        virtual FSMStateTypeID CheckTransitions(_In_ RtsGame& game) = 0;

        void Reset()
        {
            while (!m_states.empty())
                m_states.pop();

            PushState(m_defaultState);
        }

        void Update(_In_ RtsGame& game)
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
                pCurrentState->Enter(game);
            }

            pCurrentState->Update(game);

            FSMStateTypeID goalStateType = CheckTransitions(game);

            // Check if there was a state transition
            if(goalStateType != FSMState::NullFSMState)
            {
                _ASSERTE(goalStateType != CurrentState()->TypeId());
                pCurrentState->Exit(game);
                PushState(goalStateType);
                pCurrentState = CurrentState();
                pCurrentState->Enter(game);
            }
        }
        
    protected:
        void AddState(std::shared_ptr<FSMState> state) { m_stateMap[state->TypeId()] = state; }
        void PushState(FSMStateTypeID newState) { m_states.push(newState); }
        void PopState() { m_states.pop(); }
        FSMStatePtr CurrentState() { _ASSERTE(!m_states.empty()); return m_stateMap.at(m_states.top()); }

    private:
        std::map<FSMStateTypeID, FSMStatePtr> m_stateMap;
        std::stack<FSMStateTypeID> m_states;
        FSMStateTypeID m_defaultState;
        bool m_firstUpdate;
    };

    typedef std::shared_ptr<StackFSM> StackFSMPtr;
}

#endif // FSMMACHINE_H
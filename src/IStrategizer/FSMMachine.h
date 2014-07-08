#ifndef FSMMACHINE_H
#define FSMMACHINE_H

#include "FSMState.h"
#include <map>

namespace IStrategizer
{
    template<class TController, class TControllerTraits = ControllerTraits<TController>>
    class FSMMachine
    {
    public:
        FSMMachine(int defaultState, int goalState) : m_defaultState(defaultState), m_currentState(defaultState), m_goalState(goalState), m_firstUpdate(true) {}
        ~FSMMachine() { for (auto statePair : m_states) delete statePair.second; }
        void AddState(FSMState<TController>* state) { _ASSERTE(m_states.count(state->StateType()) == 0); m_states[state->StateType()] = state; }
        bool ReachedGoalState() const { return m_currentState == m_goalState; }
        
        //************************************
        // IStrategizer::FSMMachine::Reset
        // Description:	Resets the FSM machine info
        //************************************
        void Reset()
        {
            m_states.clear();
            m_currentState = NULL;
            m_defaultState = NULL;
        }

        //************************************
        // IStrategizer::FSMMachine::Update
        // Description:	Updates the current FSM machine
        // Parameter game: The current game state
        // Parameter clock: The current game time
        //************************************
        void Update(RtsGame& game, const WorldClock& clock)
        {
            // Don't do anything if you have no states
            if(m_states.empty())
                return;

            // Make sure that current state is valid
            _ASSERTE(m_states.count(m_currentState) != 0);

            // Update current state, and check for a transition
            FSMState<TController>* pCurrentState = m_states[m_currentState];
            
            // Call enter for the first state 
            if (m_firstUpdate)
            {
                m_firstUpdate = false;
                pCurrentState->Enter(game, clock);
            }

            pCurrentState->Update(game, clock);

            int oldStateType = m_currentState;
            int goalStateType = pCurrentState->CheckTransitions(game, clock);

            // Check if there was a transition
            if(goalStateType != oldStateType)
            {
                _ASSERTE(m_states.count(goalStateType) != 0);
                pCurrentState->Exit(game, clock);
                pCurrentState = m_states[goalStateType];
                m_currentState = goalStateType;
                pCurrentState->Enter(game, clock);
            }
        }

    private:
        std::map<int, FSMState<TController>*> m_states;
        bool m_firstUpdate;
        int m_defaultState;
        int m_currentState;
        int m_goalState;
    };
}

#endif // FSMMACHINE_H
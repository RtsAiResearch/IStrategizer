#ifndef FSMSTATE_H
#define FSMSTATE_H

#include "RtsGame.h"
#include <memory>

namespace IStrategizer
{
    typedef unsigned FSMStateTypeID;

    class FSMState : public EngineObject
    {
    public:
        FSMState(EngineObject* pController) : m_pController(pController) {}
        virtual FSMStateTypeID TypeId() = 0;
        virtual void Enter(_In_ RtsGame& game) {}
        virtual void Exit(_In_ RtsGame& game) {}
        virtual void Update(_In_ RtsGame& game) {}

        static const FSMStateTypeID NullFSMState = 0;

    private:
        EngineObject* m_pController;
    };

    typedef std::shared_ptr<FSMState> FSMStatePtr;
}

#endif // FSMSTATE_H
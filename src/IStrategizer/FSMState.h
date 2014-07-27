#ifndef FSMSTATE_H
#define FSMSTATE_H

#include "EngineDefs.h"
#include "RtsGame.h"
#include <memory>

namespace IStrategizer
{
    typedef unsigned FSMStateTypeID;

    class FSMState : public EngineObject
    {
    public:
        FSMState(FSMStateTypeID typeId, const char* pName, EngineObject* pController) :
            m_typeId(typeId),
            m_pName(pName),
            m_pController(pController)
        {}

        FSMStateTypeID TypeId() const { return m_typeId; }
        virtual void Enter() {}
        virtual void Exit() {}
        virtual void Update() {} 
        std::string ToString(bool minimal = false) { return m_pName; }
        static const FSMStateTypeID NullFSMState = 0;

    protected:
        DISALLOW_COPY_AND_ASSIGN(FSMState);

        EngineObject* m_pController;
        const FSMStateTypeID m_typeId;
        const char* m_pName;
    };

    typedef std::shared_ptr<FSMState> FSMStatePtr;
    typedef std::shared_ptr<const FSMState> ConstFSMStatePtr;
}

#endif // FSMSTATE_H
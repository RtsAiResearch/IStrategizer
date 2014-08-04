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
            m_pController(pController),
            m_pParent(nullptr)
        {}

        FSMStateTypeID TypeId() const { return m_typeId; }
        virtual void Enter() {}
        virtual void Exit() {}
        virtual void Update() {}
        void Parent(_In_ FSMState* pParent) { m_pParent = pParent; }
        const FSMState* Parent() { return m_pParent; }
        virtual void DebugDraw() {}

        std::string ToString(bool minimal = false) const
        {
            if (minimal)
                return m_pName;
            else
            {
                char str[128];
                sprintf_s(str, "%s.%s", (m_pController ? m_pController->ToString().c_str() : ""), m_pName);
                return  str;
            }
        }

        static const FSMStateTypeID NullFSMState = 0;

    protected:
        DISALLOW_COPY_AND_ASSIGN(FSMState);

        EngineObject* m_pController;
        const FSMStateTypeID m_typeId;
        const char* m_pName;
        FSMState* m_pParent;
    };

    typedef std::shared_ptr<FSMState> FSMStatePtr;
    typedef std::shared_ptr<const FSMState> ConstFSMStatePtr;
}

#endif // FSMSTATE_H
#ifndef ABSTRACTRETAINER_H
#define ABSTRACTRETAINER_H

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif
#include "CaseBaseEx.h"

namespace IStrategizer
{
    class AbstractRetainer : public EngineComponent
    {
    public:
        AbstractRetainer(const char* p_name) : 
            EngineComponent(p_name) {}

        virtual void ReadCaseBase() = 0;
        virtual void Flush() = 0;
        void Retain(_In_ CaseEx* pCase);
        CaseBaseEx* CaseBase() { return &m_casebase; }

    protected:
        CaseBaseEx m_casebase;
    };
}
#endif ABSTRACTRETAINER_H

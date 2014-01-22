#ifndef ABSTRACTRETAINER_H
#define ABSTRACTRETAINER_H

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif

namespace IStrategizer
{
    class CaseEx;
    class CaseBaseEx;

    class AbstractRetainer : public EngineComponent
    {
    public:
        AbstractRetainer(RtsGame& p_RtsGame, const char* p_name) : EngineComponent(p_RtsGame, p_name), _caseBase(nullptr) {}
        virtual void ReadCaseBase() = 0;
        virtual void Flush() = 0;
        virtual void Retain(const CaseEx* p_case) = 0;
        CaseBaseEx* CaseBase() { return _caseBase; }

    protected:
        CaseBaseEx* _caseBase;
    };
}
#endif ABSTRACTRETAINER_H

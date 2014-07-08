#ifndef ABSTRACTRETAINER_H
#define ABSTRACTRETAINER_H

#include "EngineObject.h"
#include "CaseBaseEx.h"

namespace IStrategizer
{
    class AbstractRetainer : public EngineObject
    {
    public:
        virtual void ReadCaseBase() = 0;
        virtual void Flush() = 0;
        void Retain(_In_ CaseEx* pCase);
        CaseBaseEx* CaseBase() { return &m_casebase; }

    protected:
        CaseBaseEx m_casebase;
    };
}
#endif ABSTRACTRETAINER_H

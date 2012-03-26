#ifndef ABSTRACTRETAINER_H
#define ABSTRACTRETAINER_H

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif

#include <string>
using namespace std;

class CaseEx;
class CaseBaseEx;

namespace OLCBP
{
    class AbstractRetainer : public EngineComponent
    {
    public:
                            AbstractRetainer(const char* p_name) : EngineComponent(p_name), _caseBase(NULL) {}
        virtual void	    ReadCaseBase() = 0;
        virtual void	    Flush() = 0;
        virtual void        Retain(const CaseEx* p_case) = 0;
		CaseBaseEx*			CaseBase() { return _caseBase; }

	protected:
		CaseBaseEx* _caseBase;
    };
}
#endif	ABSTRACTRETAINER_H
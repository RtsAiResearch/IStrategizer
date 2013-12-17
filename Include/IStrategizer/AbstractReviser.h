#ifndef ABSTRACTREVISER_H
#define ABSTRACTREVISER_H

class CaseBaseEx;
class CaseEx;

namespace IStrategizer
{
    class AbstractReviser
    {
    public:
        virtual void Revise(CaseEx* p_case, bool p_succeeded) = 0;
    };
}

#endif	// ABSTRACTREVISER_H
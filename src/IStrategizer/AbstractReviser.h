#ifndef ABSTRACTREVISER_H
#define ABSTRACTREVISER_H

namespace IStrategizer
{
    class CaseBaseEx;
    class CaseEx;

    class AbstractReviser
    {
    public:
        virtual void Revise(CaseEx* pCase, bool succeeded) = 0;
    };
}

#endif // ABSTRACTREVISER_H

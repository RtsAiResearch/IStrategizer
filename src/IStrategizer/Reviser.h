#ifndef REVISER_H
#define REVISER_H

#ifndef ABSTRACTREVISER_H
#include "AbstractReviser.h"
#endif

namespace IStrategizer
{
    class Reviser : public AbstractReviser
    {
    public:
        void Revise(CaseEx* pCase, bool succeeded);
    };
}

#endif // REVISER_H

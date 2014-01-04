///> [Serializable]
#ifndef CASEBASEEX_H
#define CASEBASEEX_H

#ifndef SVECTOR_H
#include "SVector.h"
#endif

#ifndef CASEEX_H
#include "CaseEx.h"
#endif

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

namespace IStrategizer
{
    ///> class=CaseBaseEx
    class CaseBaseEx : public Serialization::UserObject
    {
        OBJECT_SERIALIZABLE(CaseBaseEx);

    public:
        ///> type=vector(CaseEx*)
        Serialization::SVector<CaseEx*> CaseContainer;

    protected:
        void InitializeAddressesAux() { AddMemberAddress(1, &CaseContainer); }
    };
}

#endif // CASEBASEEX_H

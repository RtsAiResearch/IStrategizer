///> [Serializable]
#ifndef CASEBASEEX_H
#define CASEBASEEX_H

#include "SVector.h"
#include "CaseEx.h"
#include "UserObject.h"

namespace IStrategizer
{
    ///> class=CaseBaseEx
    class CaseBaseEx : public Serialization::UserObject
    {
        OBJECT_SERIALIZABLE(CaseBaseEx);
        OBJECT_MEMBERS(1, &CaseContainer);

    public:
        ///> type=vector(CaseEx*)
        Serialization::SVector<CaseEx*> CaseContainer;
    };
}

#endif // CASEBASEEX_H

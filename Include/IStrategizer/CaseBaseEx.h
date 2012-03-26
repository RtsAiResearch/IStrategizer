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

///> class=CaseBaseEx
class CaseBaseEx : public UserObject
{
public:
    ///> type=vector(CaseEx*)
    Serialization::SVector<CaseEx*> CaseContainer;
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string  	TypeName() 	{ return "CaseBaseEx"; }
	int     	TypeSize()  { return sizeof(CaseBaseEx); }
	UserObject* Prototype() { return new CaseBaseEx; }	
	
protected:
    void InitializeAddressesAux() { AddMemberAddress(1, &CaseContainer); }
	//----------------------------------------------------------------------------------------------
};

#endif	// CASEBASEEX_H
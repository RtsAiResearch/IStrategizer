///> [Serializable]
#ifndef TRUE_H
#define TRUE_H

#ifndef CONDITIONEX_H
#include "ConditionEx.h"
#endif

///> class=True
///> parent=ConditionEx
class True : public ConditionEx
{
public:
	True() : ConditionEx(PLAYER_Self, CONDEX_False) {}
	bool Evaluate() { return true; }
	bool Consume(int p_amount) { return true; }
	//----------------------------------------------------------------------------------------------
	// Serialization
	string              TypeName()  { return "True"; }
	int                 TypeSize()  { return sizeof(True); }
	UserObject*         Prototype() { return new True; }
};

#endif // TRUE_H
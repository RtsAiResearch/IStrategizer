///> [Serializable]
#ifndef FALSE_H
#define FALSE_H

#ifndef CONDITIONEX_H
#include "ConditionEx.h"
#endif

///> class=False
///> parent=ConditionEx
class False : public ConditionEx
{
public:
	False() : ConditionEx(PLAYER_Self, CONDEX_False) {}
	bool Evaluate() { return false; }
	bool Consume(int p_amount) { return true; }
	//----------------------------------------------------------------------------------------------
	// Serialization
	string              TypeName()  { return "False"; }
	int                 TypeSize()  { return sizeof(False); }
	UserObject*         Prototype() { return new False; }
};

#endif // FALSE_H
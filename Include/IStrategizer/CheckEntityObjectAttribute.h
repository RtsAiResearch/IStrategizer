///> [Serializable]
#ifndef CHECKENTITYOBJECTATTRIBUTE_H
#define CHECKENTITYOBJECTATTRIBUTE_H

#include "ConditionEx.h"

///> class=CheckEntityObjectAttribute
///> parent=ConditionEx
class CheckEntityObjectAttribute : public ConditionEx
{
private:
	int _currentValue;
	int *_entityObjectIdPtr;
public:
                CheckEntityObjectAttribute() {}
				CheckEntityObjectAttribute(PlayerType p_player, int p_entityObjectId, int p_attributeId, int p_operator, int p_value);
				CheckEntityObjectAttribute(PlayerType p_player, int *p_entityObjectIdPtr, int p_attributeId, int p_operator, int p_value);
	int			CurrentValue() const { return _currentValue; }
	bool		Evaluate();
    void        Copy(IClonable* p_dest);
	bool		Consume(int p_amount) { return true; }
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "CheckEntityObjectAttribute"; }
    int         TypeSize()  { return sizeof(CheckEntityObjectAttribute); }
    UserObject* Prototype() { return new CheckEntityObjectAttribute; }	
	//----------------------------------------------------------------------------------------------
};
#endif	// CHECKENTITYOBJECTATTRIBUTE_H
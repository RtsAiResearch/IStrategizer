///> [Serializable]
#ifndef CHECKENTITYCLASSATTRIBUTE_H
#define CHECKENTITYCLASSATTRIBUTE_H

#include "ConditionEx.h"

///> class=CheckEntityClassAttribute
///> parent=ConditionEx
class CheckEntityClassAttribute : public ConditionEx
{
private:
	int _currentValue;

public:
                CheckEntityClassAttribute() {}
				CheckEntityClassAttribute(PlayerType p_player, int p_entityClssId, int p_attributeId, int p_operator, int p_value);
	int			CurrentValue() const { return _currentValue; }
	bool		Evaluate();
	void		CheckForSingleEntity(EntityClassType p_entityClassType);
	void		CheckForAllEntities();
    void        Copy(IClonable* p_dest);
	bool		Consume(int p_amount) { return true; }
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "CheckEntityClassAttribute"; }
	int         TypeSize()  { return sizeof(CheckEntityClassAttribute); }
    UserObject* Prototype() { return new CheckEntityClassAttribute; }	
    //----------------------------------------------------------------------------------------------
};

#endif	// CHECKENTITYCLASSATTRIBUTE_H
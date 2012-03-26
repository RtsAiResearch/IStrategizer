///> [Serializable]
#ifndef ENTITYOBJECTEXIST_H
#define ENTITYOBJECTEXIST_H

#include "ConditionEx.h"

///> class=EntityObjectExist
///> parent=ConditionEx
class EntityObjectExist : public ConditionEx
{
	int *_entityIdPtr;
public:
                EntityObjectExist() {}
				EntityObjectExist(PlayerType p_player, int p_entityId) : ConditionEx(p_player, CONDEX_EntityObjectExist), _entityIdPtr(NULL) { _conditionParameters[PARAM_EntityObjectId] = p_entityId; }
				EntityObjectExist(PlayerType p_player, int* p_entityIdPtr) : ConditionEx(p_player, CONDEX_EntityObjectExist), _entityIdPtr(p_entityIdPtr) {}
	bool		Evaluate();
	bool		Consume(int p_amount) { return true; }
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "EntityObjectExist"; }
	int         TypeSize()  { return sizeof(EntityObjectExist); }
    UserObject* Prototype() { return new EntityObjectExist; }	
    //----------------------------------------------------------------------------------------------
};
#endif	// ENTITYOBJECTEXIST_H
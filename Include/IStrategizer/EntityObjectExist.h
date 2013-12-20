///> [Serializable]
#ifndef ENTITYOBJECTEXIST_H
#define ENTITYOBJECTEXIST_H

#include "ConditionEx.h"

namespace IStrategizer
{
	///> class=EntityObjectExist
	///> parent=ConditionEx
	class EntityObjectExist : public ConditionEx
	{
		OBJECT_SERIALIZABLE(EntityObjectExist);
		
	private:
		int *_entityIdPtr;

	public:
		EntityObjectExist() {}
		EntityObjectExist(PlayerType p_player, int p_entityId) : ConditionEx(p_player, CONDEX_EntityObjectExist), _entityIdPtr(nullptr) { _conditionParameters[PARAM_EntityObjectId] = p_entityId; }
		EntityObjectExist(PlayerType p_player, int* p_entityIdPtr) : ConditionEx(p_player, CONDEX_EntityObjectExist), _entityIdPtr(p_entityIdPtr) {}
		bool		Evaluate();
		bool		Consume(int p_amount) { return true; }
	};
}

#endif	// ENTITYOBJECTEXIST_H
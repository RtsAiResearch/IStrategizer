///> [Serializable]
#ifndef BUILDINGDESCRIPTION_H
#define BUILDINGDESCRIPTION_H

#include "UserObject.h"

namespace IStrategizer
{
	class GameEntity;
	///> class=BuildingDescription
	class BuildingDescription : public Serialization::UserObject
    {
		OBJECT_SERIALIZABLE(BuildingDescription);

	public:
		///> type=int
		int		m_numberOfBuildings;

		///> type=int
		int		m_numberOfCriticalBuildings;

		void	AddEntity(GameEntity *p_entity);
		void	RemoveEntity(GameEntity *p_entity);
		void	Clear();
		float	GetDistance(BuildingDescription *p_other);
	
	protected:
		void        InitializeAddressesAux();
    };
}

#endif // BUILDINGDESCRIPTION_H
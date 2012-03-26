///> [Serializable]
#ifndef BUILDINGDESCRIPTION_H
#define BUILDINGDESCRIPTION_H

#include "UserObject.h"
using namespace Serialization;

namespace IStrategizer
{
	class GameEntity;
	///> class=BuildingDescription
	class BuildingDescription : public UserObject
    {
	private:
		///> type=int
		int		m_numberOfBuildings;
		///> type=int
		int		m_totalDamage;
		///> type=int
		int		m_totalHP;
		///> type=int
		int		m_numberOfCriticalBuildings;
	public:
		void	AddEntity(GameEntity *p_entity);
		void	RemoveEntity(GameEntity *p_entity);
		void	Clear();
		double	GetDistance(BuildingDescription *p_other);
		//----------------------------------------------------------------------------------------------
		// Serialization
	public:
		string      TypeName()  { return "BuildingDescription"; }
		int         TypeSize()  { return sizeof(BuildingDescription); }
		UserObject* Prototype() { return new BuildingDescription; }	
	protected:
		void        InitializeAddressesAux();
		//----------------------------------------------------------------------------------------------
    };
}

#endif // BUILDINGDESCRIPTION_H
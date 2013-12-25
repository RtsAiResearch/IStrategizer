///> [Serializable]
#ifndef FORCEDESCRIPTION_H
#define FORCEDESCRIPTION_H

#include "MetaData.h"
#include "UserObject.h"
#include "SVector.h"

namespace IStrategizer
{
	class GameEntity;

	///> class=ForceDescription
	class ForceDescription : public Serialization::UserObject
    {
		OBJECT_SERIALIZABLE(ForceDescription);

	public:
		///> type=int
		int	m_numberOfUnits;

		///> type=int
		int	m_totalHP;
		
		///> type=int
		int	m_totalDamage;

	public:
		void	AddEntity(GameEntity *p_entity);
		void	RemoveEntity(GameEntity *p_entity);
		void	Clear();
		double	GetDistance(ForceDescription *p_other);

	protected:
		void	InitializeAddressesAux();
    };
}

#endif // FORCEDESCRIPTION_H
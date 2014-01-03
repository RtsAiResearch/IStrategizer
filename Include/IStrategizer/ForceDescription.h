#ifndef FORCEDESCRIPTION_H
#define FORCEDESCRIPTION_H

#include "MetaData.h"

namespace IStrategizer
{
	class GameEntity;

	class ForceDescription
    {
	public:
		int	m_numberOfUnits;
		int	m_totalHP;
		int	m_totalDamage;

	public:
		void	AddEntity(GameEntity *p_entity);
		void	RemoveEntity(GameEntity *p_entity);
		void	Clear();
		float	GetDistance(ForceDescription *p_other);
    };
}

#endif // FORCEDESCRIPTION_H
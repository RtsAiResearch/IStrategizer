#ifndef RESOURCEDESCRIPTION_H
#define RESOURCEDESCRIPTION_H


#include "MetaData.h"
using namespace MetaData;

namespace IStrategizer
{
	class GameEntity;
    class ResourceDescription
    {
		int		m_numberOfSources[COUNT(ResourceType)];
		int		m_totalResources[COUNT(ResourceType)];//NOT USED
	public:
		void	AddEntity(GameEntity *p_entity);
		void	RemoveEntity(GameEntity *p_entity);
		void	Clear();
		double	GetDistance(ResourceDescription *p_other);
    };
}

#endif // RESOURCEDESCRIPTION_H
///> [Serializable]
#ifndef CELLFEATURE_H
#define CELLFEATURE_H

#include "ForceDescription.h"
#include "BuildingDescription.h"
#include "ResourceDescription.h"
#include "MetaData.h"
#include "UserObject.h"
#include "EngineData.h"
#include  <map>

namespace IStrategizer
{
	class GameEntity;

	///> class=CellFeature
	class CellFeature : public Serialization::UserObject
    {
		OBJECT_SERIALIZABLE(CellFeature);

	public:
		///> type=ResourceDescription
		ResourceDescription		m_resourceDescription;
		///> type=ForceDescription
		ForceDescription		m_alliedForceDescription;
		///> type=ForceDescription
		ForceDescription		m_enemyForceDescription;
		///> type=BuildingDescription
		BuildingDescription		m_alliedBuildingDescription;
		///> type=BuildingDescription
		BuildingDescription		m_enemyBuildingDescription;

	public:
				CellFeature() { Clear(); }
				CellFeature(const PlanStepParameters& p_parameters);
		void	AddEntity(GameEntity *p_entity,bool p_isAllied);
		void	RemoveEntity(GameEntity *p_entity, bool p_isAllied);
		void	Clear();
		void	To(PlanStepParameters& p_parameters);
		float	GetDistance(CellFeature *p_other);

	protected:
		void	InitializeAddressesAux();
    };
}

#endif // CELLFEATURE_H
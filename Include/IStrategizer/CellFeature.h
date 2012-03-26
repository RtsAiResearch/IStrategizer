///> [Serializable]
#ifndef CELLFEATURE_H
#define CELLFEATURE_H

#include "ForceDescription.h"
#include "BuildingDescription.h"
#include "ResourceDescription.h"
#include "MetaData.h"
#include "UserObject.h"
#include  <map>

namespace IStrategizer
{
	using namespace MetaData;
	using namespace Serialization;
	using namespace std;

	class GameEntity;
	///> class=CellFeature
	class CellFeature : public UserObject
    {
	public:
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
		void							AddEntity(GameEntity *p_entity,bool p_isAllied);
		void							RemoveEntity(GameEntity *p_entity, bool p_isAllied);
		void							Clear();
		double							GetDistance(CellFeature *p_other);
		//----------------------------------------------------------------------------------------------
		// Serialization
	public:
		string      TypeName()  { return "CellFeature"; }
		int         TypeSize()  { return sizeof(CellFeature); }
		UserObject* Prototype() { return new CellFeature; }	
	protected:
		void        InitializeAddressesAux();
		//----------------------------------------------------------------------------------------------
    };
}

#endif // CELLFEATURE_H
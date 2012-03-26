///> [Serializable]
#ifndef FORCEDESCRIPTION_H
#define FORCEDESCRIPTION_H


#include "MetaData.h"
#include "UserObject.h"
#include "SVector.h"
using namespace MetaData;
using namespace Serialization;

namespace IStrategizer
{
	class GameEntity;
	///> class=ForceDescription
	class ForceDescription : public UserObject
    {
	public:
		///> type=int
		int							m_numberOfUnits;
		///> type=int
		int							m_totalHP;
		///> type=int
		int							m_totalDamage;
		//////> type=vector(int)
		//Serialization::SVector<int>	m_numberOfUnitsType;

	public:
		void					AddEntity(GameEntity *p_entity);
		void					RemoveEntity(GameEntity *p_entity);
		void					Clear();
		double					GetDistance(ForceDescription *p_other);
		//----------------------------------------------------------------------------------------------
		// Serialization
	public:
		string      TypeName()  { return "ForceDescription"; }
		int         TypeSize()  { return sizeof(ForceDescription); }
		UserObject* Prototype() { return new ForceDescription; }	
	protected:
		void        InitializeAddressesAux();
		//----------------------------------------------------------------------------------------------
    };
}

#endif // FORCEDESCRIPTION_H
#ifndef METADATA_H
#define METADATA_H

#include "AbstractionMetaData.h"
#include "TypesMetaData.h"
#include "AttributesMetaData.h"

namespace MetaData
{
	void Init();
	void InitAbstractionMetaData();
	void InitTypesMetaData();
	void InitAttributesMetaData();

	struct ForceDescriptionEx
	{	
		ForceDescriptionEx() {};
		ForceDescriptionEx(ForceSizeType p_size, PercentageType p_rangedPercentage,PercentageType p_goldPercentage,PercentageType p_lightPercentage,PercentageType p_airResistancePercentage,PercentageType p_buildingsPercentage,PercentageType p_flyingUnitsPercentage)
		{
			Size = p_size;
			RangedPercentage = p_rangedPercentage;
			GoldPercentage = p_goldPercentage;
			LightPercentage = p_lightPercentage;
			AirResistancePercentage = p_airResistancePercentage;
			BuildingsPercentage = p_buildingsPercentage;
			FlyingUnitsPercentage = p_flyingUnitsPercentage;
		}

		//Size of the force
		ForceSizeType Size;

		//Ranged or Non-Ranged Percentage?
		//Ranged : Archer,Catapult,Dragon
		//Non-Ranged: Footman,Knight
		PercentageType RangedPercentage;
		//Gold Or Wood Percentage? : 
		//Gold : Footman,dragon
		//Both : Knight,Archer
		//Wood: Catapult
		PercentageType GoldPercentage;

		//Light Or Heavy  Percentage?
		//Light: Footman,Archer
		//Heavy: Knight,Dragon,Catapult
		PercentageType LightPercentage;

		//Can it attack air ?
		//Yes : Archer,Dragon
		//No : Footman,Knight,Catapult
		PercentageType AirResistancePercentage;

		//Is it a defensive force which needs Towers ?
		//Yes :Guard Tower,Cannon Tower, and watch tower (20% only)
		//No :the rest
		PercentageType BuildingsPercentage;


		//Percentage of flying units !
		//Yes : Dragon
		//No : Rest
		PercentageType FlyingUnitsPercentage;

	};
}

#endif // METADATA_H
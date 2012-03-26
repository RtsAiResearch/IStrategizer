#ifndef ATTRIBUTESMETADATA_H
#define ATTRIBUTESMETADATA_H

//---------------------------------------------------------------------------
#define Prefix_EntityObjectAttribute EOATTRIBUTE
enum EntityObjectAttribute
{
    EOATTRIBUTE_State = 3000000,
	EOATTRIBUTE_START = 3000000,
	EOATTRIBUTE_Health,
    EOATTRIBUTE_ConcretePosition,
    EOATTRIBUTE_PosFeatureGoldMinesCount,
    EOATTRIBUTE_PosFeatureTreesCount,
    EOATTRIBUTE_PosFeatureBaseDistance,
	EOATTRIBUTE_EntityClassType,
    EOATTRIBUTE_END
};

//---------------------------------------------------------------------------
#define Prefix_EntityClassAttribute ECATTRIBUTE
enum EntityClassAttribute
{
    ECATTRIBUTE_Count  = 3001000,
	ECATTRIBUTE_START  = 3001000,
	ECATTRIBUTE_END
};

//---------------------------------------------------------------------------
#define Prefix_EntityObjectHealth EOHEALTH
enum EntityObjectHealth
{
	EOHEALTH_10		= 3002000,
	EOHEALTH_START  = 3002000,
	EOHEALTH_20,
	EOHEALTH_30,
	EOHEALTH_40,
	EOHEALTH_50,
	EOHEALTH_60,
	EOHEALTH_70,
	EOHEALTH_80,
	EOHEALTH_90,
	EOHEALTH_100,
	EOHEALTH_END
};

//---------------------------------------------------------------------------
#define Prefix_AbstractPositionAttribute APATTRIBUTE
enum AbstractPositionAttribute
{
	APATTRIBUTES_NearGold	= 3003000,
	APATTRIBUTES_START		= 3003000,
	APATTRIBUTES_NearWood,
	APATTRIBUTES_NearSameBuilding,
	APATTRIBUTES_OnBorder,
	APATTRIBUTES_OnEnemyRoute,
	APATTRIBUTES_END
};

//---------------------------------------------------------------------------
#define Prefix_PlayerAttribute PATTRIBUTE
enum PlayerAttribute
{
	PATTRIBUTE_EntitiesCount	= 3004000,
	PATTRIBUTE_START			= 3004000,
	PATTRIBUTE_END
};
#endif // ATTRIBUTESMETADATA_H
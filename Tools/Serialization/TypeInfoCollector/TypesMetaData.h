#ifndef TYPESMETADATA_H
#define TYPESMETADATA_H

//---------------------------------------------------------------------------
#define Prefix_EntityClassType ECLASS
enum EntityClassType
{
	ECLASS_Grunt	= 2000000,
	ECLASS_START	= 2000000,
	ECLASS_Peon,
    ECLASS_Catapult,
    ECLASS_Ogre,
    ECLASS_AxeThrower,
    ECLASS_Dragon,
    ECLASS_PigFarm,
    ECLASS_OgreMound,
    ECLASS_DragonRoost,
    ECLASS_GreatHall,
    ECLASS_TrollLumberMill,
    ECLASS_Stronghold,
    ECLASS_Fortress,
    ECLASS_OrcBarracks,
    ECLASS_OrcWatchTower,
    ECLASS_OrcBlacksmith,
    ECLASS_OrcGuardTower,
    ECLASS_OrcCannonTower,
	ECLASS_AltarOfStorms,
    ECLASS_GoldMine,
    ECLASS_END,
	ECLASS_AnyUnit,
};

//---------------------------------------------------------------------------
#define Prefix_UpgradeType UPGRADE
enum UpgradeType
{
    UPGRADE_BattleAxe1  = 2001000,
	UPGRADE_START       = 2001000,
	UPGRADE_BattleAxe2,
    UPGRADE_ThrowingAxe1,
    UPGRADE_ThrowingAxe2,
    UPGRADE_OrcShield1,
    UPGRADE_OrcShield2,
    UPGRADE_Catapult1,
    UPGRADE_Catapult2,
    UPGRADE_Berserker,
    UPGRADE_LightAxes,
    UPGRADE_BerserkerScouting,
    UPGRADE_BerserkerRegeneration,
	UPGRADE_OgreMage,
	UPGRADE_Stronghold,
	UPGRADE_Fortress,
    UPGRADE_END
};

//---------------------------------------------------------------------------
#define Prefix_ResourceType RESOURCE
enum ResourceType
{
    RESOURCE_Gold   = 2002000,
	RESOURCE_START  = 2002000,
	RESOURCE_Wood,
    RESOURCE_Food,
    RESOURCE_END
};

//---------------------------------------------------------------------------
#define Prefix_PlayerType PLAYER
enum PlayerType
{
    PLAYER_P1		= 2003000,
	PLAYER_START	= 2003000,
	PLAYER_P2,
	PLAYER_P3,
	PLAYER_P4,
	PLAYER_P5,
	PLAYER_P6,
	PLAYER_P7,
    PLAYER_P8,
    PLAYER_P9,
    PLAYER_Any,
    PLAYER_END
};

//---------------------------------------------------------------------------
#define Prefix_AttackType ATTACK
enum AttackType
{
	ATTACK_Defeat	    = 2004000,
	ATTACK_START		= 2004000,
    //ATTACK_Attrition,
    ATTACK_END
    
};

//---------------------------------------------------------------------------
#define Prefix_ForceSizeType FORCESIZE
enum ForceSizeType
{
    FORCESIZE_UnaryForce    = 2005000,
	FORCESIZE_START         = 2005000,
	FORCESIZE_TinyForce,
    FORCESIZE_SmallForce,
    FORCESIZE_ModerateForce,
    FORCESIZE_BigForce,
    FORCESIZE_LargeForce,
    FORCESIZE_HugeForce,
    FORCESIZE_GiganticForce,
    FORCESIZE_END
};

//---------------------------------------------------------------------------
#define Prefix_FilterType FILTER
enum FilterType
{
    FILTER_AttackingUnit   = 2006000,
	FILTER_START            = 2006000,
	FILTER_AnyEntity,
    FILTER_AnyBuilding,
    FILTER_AnyUnit,
    FILTER_END
};

//---------------------------------------------------------------------------
#define Prefix_GoalTypeEx GOALEX
enum GoalTypeEx
{
    GOALEX_WinWargus  = 2007000,				
	GOALEX_START      = 2007000,
	GOALEX_BuildBase,
    GOALEX_TrainForce,
    GOALEX_AttackEnemy,
    GOALEX_END
};

//---------------------------------------------------------------------------
#define Prefix_ActionTypeEx ACTIONEX
enum ActionTypeEx
{
    ACTIONEX_Train = 2008000,
	ACTIONEX_START = 2008000,
	ACTIONEX_Build,
    ACTIONEX_Research,
    ACTIONEX_AttackGround,
	ACTIONEX_AttackEntity,
    ACTIONEX_END
};

//---------------------------------------------------------------------------
#define Prefix_ConditionType CONDEX
enum ConditionTypeEx
{
    CONDEX_EntityClassExist	= 2009000,
	CONDEX_START              = 2009000,
	CONDEX_EntityObjectExist,
    CONDEX_CheckEntityClassAttribute,
    CONDEX_CheckEntityObjectAttribute,
    CONDEX_CheckEntityObjectSimilarity,
    CONDEX_ResearchExist,
    CONDEX_ResourceExist,
    CONDEX_CheckColonyFilterCount,
    CONDEX_CheckPositionFilterCount,
    CONDEX_END
};

//---------------------------------------------------------------------------
#define Prefix_ParameterType PARAM
enum ParameterType
{
	PARAM_EntityClassId	= 2010000,
	PARAM_START			= 2010000,
	PARAM_Amount,
	PARAM_EntityObjectId,
	PARAM_AttributeId,
	PARAM_OperatorId,
	PARAM_Value,
	PARAM_UpgradeId,
	PARAM_ResourceId,
	PARAM_FilterTypeId,
	PARAM_AbstractPositionId,
	PARAM_BuildingClassId,
	PARAM_WorkerClassId,
	PARAM_WorkerStateId,
	PARAM_TargetEntityClassId,
	PARAM_TargetPlayerId,
	PARAM_EntityClassAttributes,
	PARAM_StrategyTypeId,
	PARAM_BaseTypeId,
	PARAM_ForceSizeId,
	PARAM_ForceDescription,
	PARAM_AttackTypeId,
    PARAM_PosX,
    PARAM_PosY,
    PARAM_PosFeatureGoldMinesCount,
    PARAM_PosFeatureTreesCount,
    PARAM_PosFeatureBaseDistance,
	PARAM_PlayerId,
	PARAM_END
};

//---------------------------------------------------------------------------
#define Prefix_ExecutionStateType ESTATE
enum ExecutionStateType
{
	ESTATE_NotPrepared	= 2011000,
	ESTATE_START		= 2011000,
	ESTATE_Pending,	
	ESTATE_Executing,
	ESTATE_Succeeded,
	ESTATE_Failed,
	ESTATE_END
};

//---------------------------------------------------------------------------
#define Prefix_RelationalOperatorType RELOP
enum RelationalOperatorType
{
    RELOP_LessThan	= 2012000,
	RELOP_START     = 2012000,
	RELOP_LessThanOrEqual,
    RELOP_GreaterThan,
    RELOP_GreaterThanOrEqual,
    RELOP_Equal,
    RELOP_NotEqual,
    RELOP_END
};

//---------------------------------------------------------------------------
#define Prefix_ObjectStateType OBJSTATE
enum ObjectStateType
{
    OBJSTATE_PendingConstruction    = 2013000,
	OBJSTATE_START                  = 2013000,
	OBJSTATE_DuringConstruction,
	OBJSTATE_Constructed,
    OBJSTATE_Idle,
    OBJSTATE_Busy,
    OBJSTATE_END
};

//---------------------------------------------------------------------------
#define Prefix_StrategyType STRTYPE
enum StrategyType
{
	STRTYPE_EarlyTierRush	= 2014000,
	STRTYPE_START			= 2014000,
	STRTYPE_END
};

//---------------------------------------------------------------------------
#define Prefix_BaseType BASETYPE
enum BaseType
{
	BASETYPE_Tier1	= 2015000,
	BASETYPE_START	= 2015000,
	BASETYPE_Tier2,
	BASETYPE_Tier3,
	BASETYPE_END
};

//---------------------------------------------------------------------------
#define Prefix_ShallowFeatureType SFEATURE
enum ShallowFeatureType
{
	SFEATURE_PigFarmCount	= 2016000,
	SFEATURE_START			= 2016000,
	SFEATURE_BarracksCount,
	SFEATURE_BlackSmithCount,
	SFEATURE_OrgeMoundCount,
	SFEATURE_TrollLumberMillCount,
	SFEATURE_GreatHallCount,
	SFEATURE_GruntCount,
	SFEATURE_PeonCount,
	SFEATURE_CatapultCount,
	SFEATURE_OgreCount,
	SFEATURE_AxeThrowerCount,
	SFEATURE_DragonCount,
	SFEATURE_WatchTowerCount,
	SFEATURE_DragonRoostCount,
	SFEATURE_GuardTowerCount,
	SFEATURE_CanonTowerCount,
	SFEATURE_AlterOfStormsCount,
	SFEATURE_GoldAmount,
	SFEATURE_WoodAmount,
	SFEATURE_FoodAmount,
	SFEATURE_AllEntitiesCount,
	SFEATURE_AttackingEntitiesCount,
	SFEATURE_END
};

//---------------------------------------------------------------------------
#define Prefix_DeepFeatureType DFEATURE
enum DeepFeatureType
{
	DFEATURE_START				= 2017000,
	DFEATURE_END
};

//---------------------------------------------------------------------------
#define Prefix_SituationType SITUATION
enum SituationType
{
	SITUATION_Misc,
	SITUATION_START	= 2018000,
	SITUATION_Beginning,
	SITUATION_Defense,
	SITUATION_Attack,
	SITUATION_DevDefense,
	SITUATION_DevAttack,
	SITUATION_BaseDevelopment,
	SITUATION_END,
};

//---------------------------------------------------------------------------
#define Prefix_PercentageType PRCNT
enum PercentageType
{
	PRCNT_0		= 2019000,
	PRCNT_START	= 2019000,
	PRCNT_10,
	PRCNT_20,
	PRCNT_30,
	PRCNT_40,
	PRCNT_50,
	PRCNT_60,
	PRCNT_70,
	PRCNT_80,
	PRCNT_90,
	PRCNT_100,
	PRCNT_END,
};

//---------------------------------------------------------------------------
#define Prefix_PhaseType PHASE
enum PhaseType
{
	PHASE_Online	= 2020000,
	PHASE_START		= 2020000,
	PHASE_Offline,
	PHASE_END,
};

//---------------------------------------------------------------------------
#define Prefix_TileFlagType TILE
enum TileFlagType
{
    TILE_Wood	= 2021000,
	TILE_START	= 2021000,
	TILE_Rock,
    TILE_Land,
    TILE_Water,
    TILE_END,
};


//---------------------------------------------------------------------------
#define Prefix_ExpressionType EXPRESSION
enum ExpressionType
{
    EXPRESSION_Leaf     = 2022000,
    EXPRESSION_START    = 2022000,
    EXPRESSION_Composite,
    EXPRESSION_END
};

//---------------------------------------------------------------------------
#define Prefix_SLevelType SLEVELTYPE
enum StepLevelType
{
	SLEVELTYPE_Low     = 2023000,
	SLEVELTYPE_START    = 2023000,
	SLEVELTYPE_Medium,
	SLEVELTYPE_High,
	SLEVELTYPE_END
};

#define TYPESMETADATA_END TILE_Water


#endif // TYPESMETADATA_H
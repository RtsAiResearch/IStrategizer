#ifndef TYPESMETADATA_H
#define TYPESMETADATA_H

#ifndef ROOTMETADATA_H
#include "RootMetaData.h"
#endif

namespace MetaData
{
	//---------------------------------------------------------------------------
#define Prefix_EntityClassType ECLASS
	enum EntityClassType
	{
		ECLASS_START	= 0x20000,
		ECLASS_END		= 0x20100,
	};
	//---------------------------------------------------------------------------
#define Prefix_ResearchType RESEARCH
	enum ResearchType
	{
		RESEARCH_START  = 0x20100,
		RESEARCH_END	= 0x20200
	};
	//---------------------------------------------------------------------------
#define Prefix_ResourceType RESOURCE
	enum ResourceType
	{
		RESOURCE_Primary	= 0x20200,
		RESOURCE_START		= 0x20200,
		RESOURCE_Secondary,
		RESOURCE_Supply,
		RESOURCE_END
	};
#define Define_ResourceType \
	Enums[RESOURCE_Primary] = "Primary"; \
	Enums[RESOURCE_Secondary] = "Secondary"; \
	Enums[RESOURCE_Supply] = "Supply";
	//---------------------------------------------------------------------------
#define Prefix_PlayerType PLAYER
	enum PlayerType
	{
		PLAYER_Self		= 0x20300,
		PLAYER_START	= 0x20300,
		PLAYER_Enemy,
		PLAYER_Neutral,
		PLAYER_END
	};
#define Define_PlayerType \
	Enums[PLAYER_Self] = "Self"; \
	Enums[PLAYER_Enemy] = "Enemy"; \
	Enums[PLAYER_Neutral] = "Neutral";
	//---------------------------------------------------------------------------
#define Prefix_AttackType ATTACK
	enum AttackType
	{
		ATTACK_Defeat	    = 0x20400,
		ATTACK_START		= 0x20400,
		//ATTACK_Attrition,
		ATTACK_END
	};
#define Define_AttackType \
	Enums[ATTACK_Defeat] = "Defeat";
	//---------------------------------------------------------------------------
#define Prefix_ForceSizeType FORCESIZE
	enum ForceSizeType
	{
		FORCESIZE_SmallForce    = 0x20500,
		FORCESIZE_START         = 0x20500,
		FORCESIZE_MediumForce,
		FORCESIZE_LargeForce,
		FORCESIZE_END
	};
#define Define_ForceSizeType \
	Enums[FORCESIZE_SmallForce] = "Unary Force"; \
	Enums[FORCESIZE_MediumForce] = "Medium Force"; \
	Enums[FORCESIZE_LargeForce] = "Large Force";
	//---------------------------------------------------------------------------
#define Prefix_FilterType FILTER
	enum FilterType
	{
		FILTER_AttackingUnit	= 0x20600,
		FILTER_START			= 0x20600,
		FILTER_AnyEntity,
		FILTER_AnyBuilding,
		FILTER_AnyUnit,
		FILTER_END
	};
	//---------------------------------------------------------------------------
#define Prefix_GoalType GOALEX
	enum GoalType
	{
		GOALEX_WinGame	= 0x20700,				
		GOALEX_START    = 0x20700,
		GOALEX_BuildBase,
		GOALEX_TrainForce,
		GOALEX_AttackEnemy,
		GOALEX_END
	};
#define Define_GoalType \
	Enums[GOALEX_WinGame] = "Win Game"; \
	Enums[GOALEX_BuildBase] = "Build Base"; \
	Enums[GOALEX_TrainForce] = "Train Force"; \
	Enums[GOALEX_AttackEnemy] = "Attack Enemy";
	//---------------------------------------------------------------------------
#define Prefix_ActionType ACTIONEX
	enum ActionType
	{
		ACTIONEX_Train = 0x20800,
		ACTIONEX_START = 0x20800,
		ACTIONEX_Build,
		ACTIONEX_BuildEx,
		ACTIONEX_Research,
		ACTIONEX_AttackGround,
		ACTIONEX_AttackEntity,
		ACTIONEX_END
	};
#define Define_ActionType \
	Enums[ACTIONEX_Train] = "Train"; \
	Enums[ACTIONEX_Build] = "Build v1"; \
	Enums[ACTIONEX_BuildEx] = "Build v2"; \
	Enums[ACTIONEX_Research] = "Research"; \
	Enums[ACTIONEX_AttackGround] = "Attack Ground"; \
	Enums[ACTIONEX_AttackEntity] = "Attack Entity";
	//---------------------------------------------------------------------------
#define Prefix_ConditionType CONDEX
	enum ConditionType
	{
		CONDEX_EntityClassExist		= 0x20900,
		CONDEX_START				= 0x20900,
		CONDEX_EntityObjectExist,
		CONDEX_CheckEntityClassAttribute,
		CONDEX_CheckEntityObjectAttribute,
		CONDEX_CheckEntityObjectSimilarity,
		CONDEX_ResearchDone,
		CONDEX_ResourceExist,
		CONDEX_CheckColonyFilterCount,
		CONDEX_CheckPositionFilterCount,
		CONDEX_False,
		CONDEX_True,
		CONDEX_END
	};
	//---------------------------------------------------------------------------
#define Prefix_ParameterType PARAM
	enum ParameterType
	{
		PARAM_EntityClassId	= 0x20a00,
		PARAM_START			= 0x20a00,
		PARAM_Amount,
		PARAM_EntityObjectId,
		PARAM_AttributeId,
		PARAM_OperatorId,
		PARAM_Value,
		PARAM_UpgradeId,
		PARAM_ResourceId,
		PARAM_FilterTypeId,
		PARAM_BuildingClassId,
		PARAM_WorkerClassId,
		PARAM_WorkerStateId,
		PARAM_TargetEntityClassId,
		PARAM_TargetPlayerId,
		PARAM_StrategyTypeId,
		PARAM_BaseTypeId,
		PARAM_ForceSizeId,
		PARAM_AttackTypeId,
		PARAM_PlayerId,
		PARAM_MessageTypeId,
		PARAM_TrainerObjectId,
		PARAM_TrainerClassId,
		PARAM_WorkerObjectId,
		PARAM_BuildingObjectId,
		PARAM_END
	};
#define Define_ParameterType \
	Enums[PARAM_EntityClassId] = "Entity Class"; \
	Enums[PARAM_Amount] = "Amount"; \
	Enums[PARAM_EntityObjectId] = "Entity Object"; \
	Enums[PARAM_AttributeId] = "Attribute"; \
	Enums[PARAM_OperatorId] = "Operator"; \
	Enums[PARAM_Value] = "Value"; \
	Enums[PARAM_UpgradeId] = "Upgrade"; \
	Enums[PARAM_ResourceId] = "Resource"; \
	Enums[PARAM_FilterTypeId] = "Filter Type"; \
	Enums[PARAM_BuildingClassId] = "Building Class"; \
	Enums[PARAM_WorkerClassId] = "Worker Class"; \
	Enums[PARAM_WorkerStateId] = "Worker State"; \
	Enums[PARAM_TargetEntityClassId] = "Target Entity Class"; \
	Enums[PARAM_TargetPlayerId] = "Target Player"; \
	Enums[PARAM_StrategyTypeId] = "Strategy"; \
	Enums[PARAM_BaseTypeId] = "Base Type"; \
	Enums[PARAM_ForceSizeId] = "Force Size"; \
	Enums[PARAM_AttackTypeId] = "Attack Type"; \
	Enums[PARAM_PlayerId] = "Player"; \
	Enums[PARAM_MessageTypeId] = "Message"; \
	Enums[PARAM_TrainerObjectId] = "Trainer Object"; \
	Enums[PARAM_TrainerClassId] = "Trainer Class"; \
	Enums[PARAM_WorkerObjectId] = "Worker Object"; \
	Enums[PARAM_BuildingObjectId] = "Building Object";
	//---------------------------------------------------------------------------
#define Prefix_ExecutionStateType ESTATE
	enum ExecutionStateType
	{
		ESTATE_NotPrepared	= 0x20b00,
		ESTATE_START		= 0x20b00,
		ESTATE_Pending,	
		ESTATE_Executing,
		ESTATE_Succeeded,
		ESTATE_Failed,
		ESTATE_END
	};
#define Define_ExecutionStateType \
	Enums[ESTATE_NotPrepared] = "Not Prepared"; \
	Enums[ESTATE_Pending] = "Pending"; \
	Enums[ESTATE_Executing] = "Executing"; \
	Enums[ESTATE_Succeeded] = "Succeeded"; \
	Enums[ESTATE_Failed] = "Failed"; \
	Enums[ESTATE_END] = "Nill";
	//---------------------------------------------------------------------------
#define Prefix_RelationalOperatorType RELOP
	enum RelationalOperatorType
	{
		RELOP_LessThan	= 0x20c00,
		RELOP_START     = 0x20c00,
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
		OBJSTATE_BeingConstructed	= 0x20d00,
		OBJSTATE_START				= 0x20d00,
		OBJSTATE_Idle,
		OBJSTATE_Moving,
		OBJSTATE_Constructing,
		OBJSTATE_Gathering,
		OBJSTATE_END
	};
#define Define_ObjectStateType \
	Enums[OBJSTATE_BeingConstructed] = "Being Constructed"; \
	Enums[OBJSTATE_Idle] =  "Idle"; \
	Enums[OBJSTATE_Moving] = "Moving"; \
	Enums[OBJSTATE_Constructing] = "Constructing"; \
	Enums[OBJSTATE_Gathering] = "Gathering";
	//---------------------------------------------------------------------------
#define Prefix_StrategyType STRTYPE
	enum StrategyType
	{
		STRTYPE_EarlyTierRush	= 0x20e00,
		STRTYPE_START			= 0x20e00,
		STRTYPE_END
	};
#define Define_StrategyType \
	Enums[STRTYPE_EarlyTierRush] = "Early Tier Rush";
	//---------------------------------------------------------------------------
#define Prefix_BaseType BASETYPE
	enum BaseType
	{
		BASETYPE_Tier1	= 0x20f00,
		BASETYPE_START	= 0x20f00,
		BASETYPE_Tier2,
		BASETYPE_Tier3,
		BASETYPE_END
	};
#define Define_BaseType \
	Enums[BASETYPE_Tier1] = "Tier 1"; \
	Enums[BASETYPE_Tier2] = "Tier 2"; \
	Enums[BASETYPE_Tier3] = "Tier 3";
	//---------------------------------------------------------------------------
#define Prefix_ShallowFeatureType DFEATURE
	enum ShallowFeatureType
	{
		SFEATURE_START	= 0x21000,
		SFEATURE_END	= 0x21100
	};
	//---------------------------------------------------------------------------
#define Prefix_DeepFeatureType DFEATURE
	enum DeepFeatureType
	{
		DFEATURE_START	= 0x21100,
		DFEATURE_END	= 0x21200
	};
	//---------------------------------------------------------------------------
#define Prefix_SituationType SITUATION
	enum SituationType
	{
		SITUATION_Misc	= 0x21200,
		SITUATION_START	= 0x21200,
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
		PRCNT_0		= 0x21300,
		PRCNT_START	= 0x21300,
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
#define Define_PercentageType \
	Enums[PRCNT_0] = "0%"; \
	Enums[PRCNT_10] = "10%"; \
	Enums[PRCNT_20] = "20%"; \
	Enums[PRCNT_30] = "30%"; \
	Enums[PRCNT_40] = "40%"; \
	Enums[PRCNT_50] = "50%"; \
	Enums[PRCNT_60] = "60%"; \
	Enums[PRCNT_70] = "70%"; \
	Enums[PRCNT_80] = "80%"; \
	Enums[PRCNT_90] = "90%"; \
	Enums[PRCNT_100] = "100%";
	//---------------------------------------------------------------------------
#define Prefix_PhaseType PHASE
	enum PhaseType
	{
		PHASE_Online	= 0x21400,
		PHASE_START		= 0x21400,
		PHASE_Offline,
		PHASE_END,
	};
	//---------------------------------------------------------------------------
#define Prefix_TileFlagType TILE
	enum TileFlagType
	{
		TILE_Wood	= 0x21500,
		TILE_START	= 0x21500,
		TILE_Rock,
		TILE_Land,
		TILE_Water,
		TILE_END,
	};
	//---------------------------------------------------------------------------
#define Prefix_ExpressionType EXPRESSION
	enum ExpressionType
	{
		EXPRESSION_Leaf     = 0x21600,
		EXPRESSION_START    = 0x21600,
		EXPRESSION_Composite,
		EXPRESSION_END
	};

	//---------------------------------------------------------------------------
#define Prefix_SLevelType SLEVELTYPE
	enum StepLevelType
	{
		SLEVELTYPE_Low      = 0x21700,
		SLEVELTYPE_START    = 0x21700,
		SLEVELTYPE_Medium,
		SLEVELTYPE_High,
		SLEVELTYPE_END
	};
#define Define_StepLevelType \
	Enums[SLEVELTYPE_Low] = "Low Level"; \
	Enums[SLEVELTYPE_Medium] = "Medium Level"; \
	Enums[SLEVELTYPE_High] = "High Level";
	//----------------------------------------------------------------------------------------------
#define Prefix_GoalStateType GSTATE
	enum GoalStateType
	{
		GSTATE_OpenReady	= 0x21800,
		GSTATE_START		= 0x21800,
		GSTATE_OpenWaiting,
		GSTATE_Closed,
		GSTATE_END,
	};
	//----------------------------------------------------------------------------------------------
#define Prefix_MessageType MSG
	enum MessageType
	{
		MSG_Log				= 0x21900,
		MSG_START			= 0x21900, 
		MSG_Input,
		MSG_GameExit,
		MSG_GameEnd,
		MSG_BuildingBuilt,
		MSG_EntityTrained,
		MSG_EntityAttacked,
		MSG_EntityKilled,
		MSG_EntityCreate,
		MSG_EntityDestroy,
		MSG_EntityRenegade,
		MSG_END
	};
	//---------------------------------------------------------------------------
#define Prefix_UnitAttackType UTYPE
	enum UnitAttackType
	{
		UTYPE_LandToAir		= 0x21a00,
		UTYPE_START			= 0x21a00,
		UTYPE_LandToLand,
		UTYPE_LandToBoth,
		UTYPE_AirToAir,
		UTYPE_AirToLand,
		UTYPE_AirToBoth,
		UTYPE_END
	};
}

#endif // TYPESMETADATA_H
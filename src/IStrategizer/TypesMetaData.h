#ifndef TYPESMETADATA_H
#define TYPESMETADATA_H

#ifndef ROOTMETADATA_H
#include "RootMetaData.h"
#endif

namespace IStrategizer
{
    //---------------------------------------------------------------------------
#define Prefix_EntityClassType ECLASS
    enum EntityClassType
    {
        ECLASS_START    = 0x20000,
        ECLASS_END        = 0x20100,
    };
    //---------------------------------------------------------------------------
#define Prefix_ResearchType RESEARCH
    enum ResearchType
    {
        RESEARCH_START  = 0x20100,
        RESEARCH_END    = 0x20200
    };
    //---------------------------------------------------------------------------
#define Prefix_ResourceType RESOURCE
    enum ResourceType
    {
        RESOURCE_Primary    = 0x20200,
        RESOURCE_START        = 0x20200,
        RESOURCE_Secondary,
        RESOURCE_Supply,
        RESOURCE_END
    };
#define Define_ResourceType \
    Enums[RESOURCE_Primary] = "Primary Resource Type"; \
    Enums[RESOURCE_Secondary] = "Secondary Resource Type"; \
    Enums[RESOURCE_Supply] = "Supply Resource Type";
    //---------------------------------------------------------------------------
#define Prefix_PlayerType PLAYER
    enum PlayerType
    {
        PLAYER_Self        = 0x20300,
        PLAYER_START    = 0x20300,
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
        ATTACK_Defeat        = 0x20400,
        ATTACK_START        = 0x20400,
        //ATTACK_Attrition,
        ATTACK_END
    };
#define Define_AttackType \
    Enums[ATTACK_Defeat] = "Defeat";
    //---------------------------------------------------------------------------
#define Prefix_FilterType FILTER
    enum FilterType
    {
        FILTER_AttackingUnit    = 0x20600,
        FILTER_START            = 0x20600,
        FILTER_AnyEntity,
        FILTER_AnyBuilding,
        FILTER_AnyUnit,
        FILTER_END
    };
    //---------------------------------------------------------------------------
#define Prefix_GoalType GOALEX
    enum GoalType
    {
        GOALEX_WinGame = 0x20700,                                
        GOALEX_START   = 0x20700,
        GOALEX_CollectResource,
        GOALEX_TrainForce,
        GOALEX_BuildInfrastructure,
        GOALEX_DestroyEntityType,
        GOALEX_DeployArmy,
        GOALEX_TrainArmy,
        GOALEX_END
    };
#define Define_GoalType \
    Enums[GOALEX_WinGame] = "Win Game"; \
    Enums[GOALEX_CollectResource] = "Collect Resource"; \
    Enums[GOALEX_TrainForce] = "Train Force"; \
    Enums[GOALEX_BuildInfrastructure] = "Build Infrastructure";\
    Enums[GOALEX_DestroyEntityType] = "Destroy Entity Type"; \
    Enums[GOALEX_DeployArmy] = "Deploy Army"; \
    Enums[GOALEX_TrainArmy] = "Train Army";
    //---------------------------------------------------------------------------
#define Prefix_ActionType ACTIONEX
    enum ActionType
    {
        ACTIONEX_Train = 0x20800,
        ACTIONEX_START = 0x20800,
        ACTIONEX_Build,
        ACTIONEX_Research,
        ACTIONEX_AttackGround,
        ACTIONEX_AttackEntity,
        ACTIONEX_Move,
        ACTIONEX_GatherResource,
        ACTIONEX_END
    };
#define Define_ActionType \
    Enums[ACTIONEX_Train] = "Train"; \
    Enums[ACTIONEX_Build] = "Build"; \
    Enums[ACTIONEX_Research] = "Research"; \
    Enums[ACTIONEX_AttackGround] = "Attack Ground"; \
    Enums[ACTIONEX_AttackEntity] = "Attack Entity"; \
    Enums[ACTIONEX_Move] = "Move"; \
    Enums[ACTIONEX_GatherResource] = "Gather Resource";
    //---------------------------------------------------------------------------
#define Prefix_ConditionType CONDEX
    enum ConditionType
    {
        CONDEX_EntityClassExist     = 0x20900,
        CONDEX_START                = 0x20900,
        CONDEX_ResearchDone,
        CONDEX_ResourceExist,
        CONDEX_EntityClassNearArea,
        CONDEX_False,
        CONDEX_PlayerAttributeExist,
		CONDEX_CandidateGathererExist,
        CONDEX_END
    };
#define Define_ConditionType \
    Enums[CONDEX_EntityClassExist] = "Entity Class Exist"; \
    Enums[CONDEX_ResearchDone] = "Research Done"; \
    Enums[CONDEX_ResourceExist] = "Resource Exist"; \
    Enums[CONDEX_EntityClassNearArea] = "Entity Class Near Area"; \
    Enums[CONDEX_False] = "False"; \
	Enums[CONDEX_PlayerAttributeExist] = "Player Attribute Exist"; \
	Enums[CONDEX_CandidateGathererExist] = "Candidate Gatherer Exist";
    //---------------------------------------------------------------------------
#define Prefix_ParameterType PARAM
    enum ParameterType
    {
        PARAM_EntityClassId = 0x20a00,
        PARAM_START = 0x20a00,
        PARAM_EntityObjectId,
        PARAM_AttributeId,
        PARAM_OperatorId,
        PARAM_Value,
        PARAM_ResearchId,
        PARAM_ResourceId,
        PARAM_FilterTypeId,
        PARAM_BuildingClassId,
        PARAM_WorkerClassId,
        PARAM_WorkerStateId,
        PARAM_TargetEntityClassId,
        PARAM_TargetPlayerId,
        PARAM_StrategyTypeId,
        PARAM_BaseTypeId,
        PARAM_AttackTypeId,
        PARAM_PlayerId,
        PARAM_MessageTypeId,
        PARAM_TrainerObjectId,
        PARAM_TrainerClassId,
        PARAM_WorkerObjectId,
        PARAM_BuildingObjectId,
        PARAM_ObjectStateType,
        PARAM_DeployType,
        PARAM_NumberOfPrimaryResources,
        PARAM_REALVALSTART = PARAM_NumberOfPrimaryResources,
        PARAM_NumberOfSecondaryResources,
        PARAM_NumberOfSupplyResources,
        PARAM_EnemyUnitsCount,
        PARAM_EnemyUnitsTotalHP,
        PARAM_EnemyUnitsTotalDamage,
        PARAM_AlliedUnitsCount,
        PARAM_AlliedAttackersTotalHP,
        PARAM_AlliedAttackersTotalDamage,
        PARAM_EnemyBuildingsCount,
        PARAM_EnemyCriticalBuildingsCount,
        PARAM_AlliedBuildingsCount,
        PARAM_AlliedCriticalBuildingsCount,
        PARAM_DistanceToEnemyBase,
        PARAM_DistanceToBase,
        PARAM_Distance,
        PARAM_Influnce,
        PARAM_Amount,
        PARAM_REALVALEND,
        PARAM_END = PARAM_REALVALEND
    };
#define Define_ParameterType \
    Enums[PARAM_EntityClassId] = "Entity Class"; \
    Enums[PARAM_Amount] = "Amount"; \
    Enums[PARAM_EntityObjectId] = "Entity Object"; \
    Enums[PARAM_AttributeId] = "Attribute"; \
    Enums[PARAM_OperatorId] = "Operator"; \
    Enums[PARAM_Value] = "Value"; \
    Enums[PARAM_ResearchId] = "Research Name"; \
    Enums[PARAM_ResourceId] = "Resource"; \
    Enums[PARAM_FilterTypeId] = "Filter Type"; \
    Enums[PARAM_BuildingClassId] = "Building Class"; \
    Enums[PARAM_WorkerClassId] = "Worker Class"; \
    Enums[PARAM_WorkerStateId] = "Worker State"; \
    Enums[PARAM_TargetEntityClassId] = "Target Entity Class"; \
    Enums[PARAM_TargetPlayerId] = "Target Player"; \
    Enums[PARAM_StrategyTypeId] = "Strategy"; \
    Enums[PARAM_BaseTypeId] = "Base Type"; \
    Enums[PARAM_AttackTypeId] = "Attack Type"; \
    Enums[PARAM_PlayerId] = "Player"; \
    Enums[PARAM_MessageTypeId] = "Message"; \
    Enums[PARAM_TrainerObjectId] = "Trainer Object"; \
    Enums[PARAM_TrainerClassId] = "Trainer Class"; \
    Enums[PARAM_WorkerObjectId] = "Worker Object"; \
    Enums[PARAM_BuildingObjectId] = "Building Object"; \
    Enums[PARAM_NumberOfPrimaryResources] = "Primary Resource"; \
    Enums[PARAM_NumberOfSecondaryResources] = "Secondary Resource"; \
    Enums[PARAM_NumberOfSupplyResources] = "Supply"; \
    Enums[PARAM_EnemyUnitsCount] = "Enemy Units Count"; \
    Enums[PARAM_EnemyUnitsTotalHP] = "Enemy Units Total HP"; \
    Enums[PARAM_EnemyUnitsTotalDamage] = "Enemy Units Total Damage"; \
    Enums[PARAM_AlliedUnitsCount] = "Allied Units Count"; \
    Enums[PARAM_AlliedAttackersTotalHP] = "Allied Attackers Total HP"; \
    Enums[PARAM_AlliedAttackersTotalDamage] = "Allied Attackers Total Damage"; \
    Enums[PARAM_EnemyBuildingsCount] = "Enemy Buildings Count"; \
    Enums[PARAM_EnemyCriticalBuildingsCount] = "Enemy Critical Buildings Count"; \
    Enums[PARAM_AlliedBuildingsCount] = "Allied Buildings Count"; \
    Enums[PARAM_AlliedCriticalBuildingsCount] = "Allied Critical Buildings Count"; \
    Enums[PARAM_DistanceToEnemyBase] = "Distance To Enemy Base"; \
    Enums[PARAM_DistanceToBase] = "Distance To Base"; \
    Enums[PARAM_Influnce] = "Influence"; \
    Enums[PARAM_ObjectStateType] = "Object State Type"; \
    Enums[PARAM_DeployType] = "Deploy Type";
    //---------------------------------------------------------------------------
#define Prefix_ExecutionStateType ESTATE
    enum ExecutionStateType
    {
        ESTATE_NotPrepared    = 0x20b00,
        ESTATE_START        = 0x20b00,
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
        RELOP_LessThan    = 0x20c00,
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
		OBJSTATE_DontCare = 0x20d00,
        OBJSTATE_START                = 0x20d00,
        OBJSTATE_Idle,
        OBJSTATE_Moving,
        OBJSTATE_Constructing,
        OBJSTATE_GatheringPrimary,
        OBJSTATE_GatheringSecondary,
        OBJSTATE_Training,
        OBJSTATE_Attacking,
        OBJSTATE_UnderAttack,
		OBJSTATE_BeingConstructed,
		OBJSTATE_END
    };
#define Define_ObjectStateType \
    Enums[OBJSTATE_BeingConstructed] = "Being Constructed"; \
    Enums[OBJSTATE_Idle] =  "Idle"; \
    Enums[OBJSTATE_Moving] = "Moving"; \
    Enums[OBJSTATE_Constructing] = "Constructing"; \
    Enums[OBJSTATE_GatheringPrimary] = "Gathering Primary"; \
    Enums[OBJSTATE_GatheringSecondary] = "Gathering Secondary"; \
    Enums[OBJSTATE_Training] = "Training"; \
    Enums[OBJSTATE_Attacking] = "Attacking"; \
    Enums[OBJSTATE_UnderAttack] = "Under Attack"; \
	Enums[OBJSTATE_DontCare] = "Don't Care State";
    //---------------------------------------------------------------------------
#define Prefix_StrategyType STRTYPE
    enum StrategyType
    {
        STRTYPE_EarlyTierRush    = 0x20e00,
        STRTYPE_START            = 0x20e00,
        STRTYPE_END
    };
#define Define_StrategyType \
    Enums[STRTYPE_EarlyTierRush] = "Early Tier Rush";
    //---------------------------------------------------------------------------
#define Prefix_BaseType BASETYPE
    enum BaseType
    {
        BASETYPE_Tier1    = 0x20f00,
        BASETYPE_START    = 0x20f00,
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
        SFEATURE_START    = 0x21000,
        SFEATURE_END    = 0x21100
    };
    //---------------------------------------------------------------------------
#define Prefix_DeepFeatureType DFEATURE
    enum DeepFeatureType
    {
        DFEATURE_START    = 0x21100,
        DFEATURE_END    = 0x21200
    };
    //---------------------------------------------------------------------------
#define Prefix_SituationType SITUATION
    enum SituationType
    {
        SITUATION_Misc    = 0x21200,
        SITUATION_START    = 0x21200,
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
        PRCNT_0        = 0x21300,
        PRCNT_START    = 0x21300,
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
        PHASE_Online    = 0x21400,
        PHASE_START        = 0x21400,
        PHASE_Offline,
        PHASE_END,
    };
    //---------------------------------------------------------------------------
#define Prefix_TileFlagType TILE
    enum TileFlagType
    {
        TILE_Wood    = 0x21500,
        TILE_START    = 0x21500,
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
        GSTATE_OpenReady    = 0x21800,
        GSTATE_START        = 0x21800,
        GSTATE_OpenWaiting,
        GSTATE_Closed,
        GSTATE_END,
    };
    //----------------------------------------------------------------------------------------------
#define Prefix_MessageType MSG
    enum MessageType
    {
        MSG_GameActionLog    = 0x21900,
        MSG_START            = 0x21900, 
        MSG_Input,
        MSG_GameStart,
        MSG_GameEnd,
        MSG_EntityCreate,
        MSG_EntityDestroy,
        MSG_EntityRenegade,
        MSG_EntityShow,
        MSG_EntityHide,
        MSG_PlanStructureChange,
        MSG_PlanGoalSuccess,
        MSG_BaseUnderAttack,
        MSG_END
    };
#define Define_MessageType \
	Enums[MSG_GameActionLog] = "GameActionLog"; \
	Enums[MSG_Input] = "Input"; \
	Enums[MSG_GameStart] = "GameStart"; \
	Enums[MSG_GameEnd] = "GameEnd"; \
	Enums[MSG_EntityCreate] = "EntityCreate"; \
	Enums[MSG_EntityDestroy] = "EntityDestroy"; \
	Enums[MSG_EntityRenegade] = "EntityRenegade"; \
	Enums[MSG_PlanStructureChange] = "PlanStructureChange"; \
	Enums[MSG_PlanGoalSuccess] = "PlanGoalSuccess"; \
    Enums[MSG_BaseUnderAttack] = "BaseUnderAttack"; \
    Enums[MSG_EntityShow] = "EntityShow"; \
    Enums[MSG_EntityHide] = "EntityHide"; \
    //---------------------------------------------------------------------------
#define Prefix_UnitAttackType UTYPE
    enum UnitAttackType
    {
        UTYPE_LandToAir        = 0x21a00,
        UTYPE_START            = 0x21a00,
        UTYPE_LandToLand,
        UTYPE_LandToBoth,
        UTYPE_AirToAir,
        UTYPE_AirToLand,
        UTYPE_AirToBoth,
        UTYPE_END
    };

    //---------------------------------------------------------------------------
#define Prefix_DeployType DEPTYPE
    enum DeployType
    {
        DEPTYPE_Defend    = 0x21b00,
        DEPTYPE_START     = 0x21b00,
        DEPTYPE_Retreat,
        DEPTYPE_Attack,
        DEPTYPE_END
    };
#define Define_DeployType \
    Enums[DEPTYPE_Defend] = "Defend"; \
    Enums[DEPTYPE_Retreat] = "Retreat"; \
    Enums[DEPTYPE_Attack] = "Attack";
}

#endif // TYPESMETADATA_H

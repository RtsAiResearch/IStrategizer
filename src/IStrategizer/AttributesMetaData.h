#ifndef ATTRIBUTESMETADATA_H
#define ATTRIBUTESMETADATA_H

#ifndef ROOTMETADATA_H
#include "RootMetaData.h"
#endif

namespace IStrategizer
{
    //---------------------------------------------------------------------------
#define Prefix_EntityObjectProperty OP
    enum EntityObjectProperty
    {
        OP_State = 0x30000,
        OP_START = 0x30000,
        OP_Health,
        OP_Left,
        OP_Top,
        OP_OwnerId,
        OP_PosCenterX,
        OP_PosCenterY,
        OP_IsMoving,
        OP_Right,
        OP_Bottom,
        OP_TypeId,
        OP_IsGatheringPrimaryResource,
        OP_IsGatheringSecondaryResource,
		OP_IsBeingGathered,
		OP_OrderTargetId,
        OP_IsBeingHit,
        OP_IsAttacking,
        OP_IsVisible,
        OP_END
    };
#define Define_EntityObjectProperty \
    Enums[OP_State] = "State"; \
    Enums[OP_Health] = "Health"; \
    Enums[OP_Left] = "Left X Position"; \
    Enums[OP_Top] = "Top Y Position"; \
    Enums[OP_OwnerId] = "Owner"; \
    Enums[OP_PosCenterX] = "Center X Position"; \
    Enums[OP_PosCenterY] = "Center Y Position"; \
    Enums[OP_IsMoving] = "Is Moving"; \
    Enums[OP_Right] = "Right X Position"; \
    Enums[OP_Bottom] = "Bottom Y Position"; \
    Enums[OP_TypeId] = "TypeID"; \
    Enums[OP_IsGatheringPrimaryResource] = "Is Gathering Primary Rresource"; \
	Enums[OP_IsGatheringSecondaryResource] = "Is Gathering Secondary Resource"; \
	Enums[OP_IsBeingGathered] = "Is Being Gathered"; \
    Enums[OP_OrderTargetId] = "Order Target ID"; \
    Enums[OP_IsBeingHit] = "Is Being Hit"; \
    Enums[OP_IsAttacking] = "Is Attacking"; \
    Enums[OP_IsVisible] = "Is Visible"; \
    //---------------------------------------------------------------------------
#define Prefix_EntityTypeProperty TP
    enum EntityTypeProperty
    {
		TP_MaxHp = 0x30100,
		TP_START = 0x30100,
		TP_CanAttack,
		TP_CanBuild,
		TP_IsBuilding,
		TP_IsWorker,
		TP_IsAttacker,
		TP_GroundAttack,
        TP_AirAttack,
		TP_AirRange,
		TP_GroundRange,
        TP_IsMelee,
		TP_IsPrimaryResource,
		TP_IsSecondaryResource,
		TP_IsSupply,
		TP_IsProducer,
		TP_Width,
		TP_Height,
		TP_LineOfSight,
		TP_IsSpecialBuilding,
		TP_BuildTime,
		TP_END
	};
#define Define_EntityTypeProperty \
    Enums[TP_MaxHp] = "Max HP"; \
    Enums[TP_CanAttack] = "Can Attack"; \
    Enums[TP_CanBuild] = "Can Build"; \
    Enums[TP_IsBuilding] = "Is Building"; \
    Enums[TP_IsWorker] = "Is Worker"; \
    Enums[TP_IsAttacker] = "Is Attacker"; \
    Enums[TP_GroundAttack] = "Attack Damage"; \
    Enums[TP_AirRange] = "Air Weapon Range"; \
    Enums[TP_GroundRange] = "Ground Weapon Range"; \
    Enums[TP_IsMelee] = "Is Melee"; \
    Enums[TP_AirAttack] = "Air Damage"; \
    Enums[TP_IsPrimaryResource] = "Is Primary Resource"; \
    Enums[TP_IsSecondaryResource] = "Is Secondary Resource"; \
    Enums[TP_IsSupply] = "Is Supply"; \
    Enums[TP_IsProducer] = "Is Producer"; \
    Enums[TP_Width] = "Tile Width"; \
    Enums[TP_Height] = "Tile Height"; \
    Enums[TP_LineOfSight] = "Line of Sight"; \
    Enums[TP_IsSpecialBuilding] = "Is Special Building"; \
    Enums[TP_BuildTime] = "BuildTime"; \
    //---------------------------------------------------------------------------
#define Prefix_PlayerAttribute PATTR
    enum PlayerAttribute
    {
        PATTR_AlliedAttackersTotalHP = 0x30200,
        PATTR_START = 0x30200,
        PATTR_AlliedAttackersTotalDamage,
        PATTR_END
    };
#define Define_PlayerAttribute \
    Enums[PATTR_AlliedAttackersTotalHP] = "Allied Attackers Total HP"; \
    Enums[PATTR_AlliedAttackersTotalDamage] = "Allied Attackers Total Damage";
    //---------------------------------------------------------------------------
#define Prefix_RtsGameModelAttribute RTSMODATTR
    enum RtsGameModelAttribute
    {
        RTSMODATTR_GameFrame = 0x30300,
        RTSMODATTR_START = 0x30300,
        RTSMODATTR_MapArea,
        RTSMODATTR_Player_Resources_Primary,
        RTSMODATTR_Player_Resources_Secondary,
        RTSMODATTR_Player_Resources_Supply,
        RTSMODATTR_Player_Entities_NumBuildings,
        RTSMODATTR_Player_Entities_NumWorkers,
        RTSMODATTR_Player_Entities_NumAttackers,
        RTSMODATTR_Player_Entities_NumDoneResearches,
        RTSMODATTR_END
    };
#define Define_RtsGameModelAttribute \
    Enums[RTSMODATTR_GameFrame] = "GameFrame"; \
    Enums[RTSMODATTR_MapArea] = "MapArea"; \
    Enums[RTSMODATTR_Player_Resources_Primary] = "Player.Resources.Primary"; \
    Enums[RTSMODATTR_Player_Resources_Secondary] = "Player.Resources.Secondary"; \
    Enums[RTSMODATTR_Player_Resources_Supply] = "Player.Resources.Supply"; \
    Enums[RTSMODATTR_Player_Entities_NumBuildings] = "Player.Entities.NumBuildings"; \
    Enums[RTSMODATTR_Player_Entities_NumWorkers] = "Player.Entities.NumWorkers"; \
    Enums[RTSMODATTR_Player_Entities_NumAttackers] = "Player.Entities.NumAttackers"; \
    Enums[RTSMODATTR_Player_Entities_NumDoneResearches] = "Player.Entities.NumDoneResearches";
    //---------------------------------------------------------------------------
#define Prefix_RtsGameModelAttributeWeight RTSMODATTRWGHT
    enum RtsGameModelAttributeWeight
    {
        RTSMODATTRWGHT_GameFrame = 0x30400,
        RTSMODATTRWGHT_START = 0x30400,
        RTSMODATTRWGHT_MapArea,
        RTSMODATTRWGHT_Player,
        RTSMODATTRWGHT_Player_Entities,
        RTSMODATTRWGHT_Player_Entities_NumBuildings,
        RTSMODATTRWGHT_Player_Entities_NumWorkers,
        RTSMODATTRWGHT_Player_Entities_NumAttackers,
        RTSMODATTRWGHT_Player_Entities_NumDoneResearches,
        RTSMODATTRWGHT_Player_Resources,
        RTSMODATTRWGHT_Player_Resources_Primary,
        RTSMODATTRWGHT_Player_Resources_Secondary,
        RTSMODATTRWGHT_Player_Resources_Supply,
        RTSMODATTRWGHT_END
    };
}

#endif // ATTRIBUTESMETADATA_H

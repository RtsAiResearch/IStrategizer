#ifndef ATTRIBUTESMETADATA_H
#define ATTRIBUTESMETADATA_H

#ifndef ROOTMETADATA_H
#include "RootMetaData.h"
#endif

namespace IStrategizer
{
    //---------------------------------------------------------------------------
#define Prefix_EntityObjectAttribute EOATTR
    enum EntityObjectAttribute
    {
        EOATTR_State = 0x30000,
        EOATTR_START = 0x30000,
        EOATTR_Health,
        EOATTR_Left,
        EOATTR_Top,
        EOATTR_OwnerId,
        EOATTR_PosCenterX,
        EOATTR_PosCenterY,
        EOATTR_IsMoving,
        EOATTR_Right,
        EOATTR_Bottom,
        EOATTR_Type,
        EOATTR_IsGatheringPrimaryResource,
        EOATTR_IsGatheringSecondaryResource,
        EOATTR_END
    };
#define Define_EntityObjectAttribute \
    Enums[EOATTR_State] = "State"; \
    Enums[EOATTR_Health] = "Health"; \
    Enums[EOATTR_Left] = "Left X Position"; \
    Enums[EOATTR_Top] = "Top Y Position"; \
    Enums[EOATTR_OwnerId] = "Owner"; \
    Enums[EOATTR_PosCenterX] = "Center X Position"; \
    Enums[EOATTR_PosCenterY] = "Center Y Position"; \
    Enums[EOATTR_IsMoving] = "Is Moving"; \
    Enums[EOATTR_Right] = "Right X Position"; \
    Enums[EOATTR_Bottom] = "Bottom Y Position"; \
    Enums[EOATTR_Type] = "Type"; \
    Enums[EOATTR_IsGatheringPrimaryResource] = "Is Gathering Primary Rresource"; \
    Enums[EOATTR_IsGatheringSecondaryResource] = "Is Gathering Secondary Resource"; \
    //---------------------------------------------------------------------------
#define Prefix_EntityClassAttribute ECATTR
    enum EntityClassAttribute
    {
		ECATTR_MaxHp = 0x30100,
		ECATTR_START = 0x30100,
		ECATTR_CanAttack,
		ECATTR_CanBuild,
		ECATTR_IsBuilding,
		ECATTR_IsCowrad,
		ECATTR_IsAttacker,
		ECATTR_Attack,
		ECATTR_IsPrimaryResource,
		ECATTR_IsSecondaryResource,
		ECATTR_IsSupply,
		ECATTR_IsCritical,
		ECATTR_Width,
		ECATTR_Height,
		ECATTR_AirRange,
		ECATTR_GroundRange,
		ECATTR_LineOfSight,
		ECATTR_IsSpecialBuilding,
		ECATTR_BuildTime,
		ECATTR_END
	};
#define Define_EntityClassAttribute \
	Enums[ECATTR_MaxHp] = "Max HP"; \
	Enums[ECATTR_CanAttack] = "Can Attack"; \
	Enums[ECATTR_CanBuild] = "Can Build"; \
	Enums[ECATTR_IsBuilding] = "Is Building"; \
	Enums[ECATTR_IsCowrad] = "Is Coward"; \
	Enums[ECATTR_IsAttacker] = "Is Attacker"; \
	Enums[ECATTR_Attack] = "Attack Damage"; \
	Enums[ECATTR_IsPrimaryResource] = "Is Primary Resource"; \
	Enums[ECATTR_IsSecondaryResource] = "Is Secondary Resource"; \
	Enums[ECATTR_IsSupply] = "Is Supply"; \
	Enums[ECATTR_IsCritical] = "Is Critical"; \
	Enums[ECATTR_Width] = "Tile Width"; \
	Enums[ECATTR_Height] = "Tile Height"; \
	Enums[ECATTR_AirRange] = "Air Weapon Range"; \
	Enums[ECATTR_GroundRange] = "Ground Weapon Range"; \
	Enums[ECATTR_LineOfSight] = "Line of Sight"; \
	Enums[ECATTR_IsSpecialBuilding] = "Is Special Building"; \
	Enums[ECATTR_BuildTime] = "BuildTime"; \
    //---------------------------------------------------------------------------
#define Prefix_PlayerAttribute PATTR
    enum PlayerAttribute
    {
        PATTR_AlliedUnitsTotalHP = 0x30200,
        PATTR_START = 0x30200,
        PATTR_AlliedUnitsTotalDamage,
        PATTR_END
    };
#define Define_PlayerAttribute \
    Enums[PATTR_AlliedUnitsTotalHP] = "Units Total HP"; \
    Enums[PATTR_AlliedUnitsTotalDamage] = "Units Total Damage";
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

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
        ECATTR_END
    };
#define Define_EntityClassAttribute \
    Enums[ECATTR_MaxHp] = "Max HP"; \
    Enums[ECATTR_CanAttack] = "Can Attack"; \
    Enums[ECATTR_CanBuild] = "Can Build"; \
    Enums[ECATTR_IsBuilding] = "Is Building"; \
    Enums[ECATTR_IsCowrad] = "Is Coward"; \
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
    Enums[ECATTR_IsSpecialBuilding] = "Is Special Building";
    //---------------------------------------------------------------------------
#define Prefix_PlayerAttribute PATTRIBUTE
    enum PlayerAttribute
    {
        PATTRIBUTE_START = 0x30200,
        PATTRIBUTE_END
    };
}
#endif // ATTRIBUTESMETADATA_H

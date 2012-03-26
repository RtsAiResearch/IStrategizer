#ifndef ABSTRACTIONMETADATA_H
#define ABSTRACTIONMETADATA_H

//---------------------------------------------------------------------------
#define Prefix_AbstractAttackTargetEx ABSTRCTATCKTRGTEX
enum AbstractAttackTargetEx
{
    ABSTRCTATCKTRGTEX_NearestTower    = 1000000,
	ABSTRCTATCKTRGTEX_START           = 1000000,
	ABSTRCTATCKTRGTEX_NearestTownCenter,
    ABSTRCTATCKTRGTEX_NearestWoodMiner,
    ABSTRCTATCKTRGTEX_NearestGoldMiner,
    ABSTRCTATCKTRGTEX_NearestBorderBuilding,
    ABSTRCTATCKTRGTEX_AirProtectedBorderBuilding,
    ABSTRCTATCKTRGTEX_NonAirProtectedBorderBuilding,
    ABSTRCTATCKTRGTEX_LandProtectedBorderBuilding,
    ABSTRCTATCKTRGTEX_NonLandProtectedBorderBuilding,
    ABSTRCTATCKTRGTEX_ProtectedBorderBuilding,
    ABSTRCTATCKTRGTEX_NonProtectedBorderBuilding,
    ABSTRCTATCKTRGTEX_AnyUnit,
    ABSTRCTATCKTRGTEX_NearestResourceBuilding,
    ABSTRCTATCKTRGTEX_NearestPeons,
    ABSTRCTATCKTRGTEX_NearestUnit,
    ABSTRCTATCKTRGTEX_NearestGoldMineTownCenter,
    ABSTRCTATCKTRGTEX_NearestAirForce,
    ABSTRCTATCKTRGTEX_NearestLandForce,
    ABSTRCTATCKTRGTEX_MaximumDamage,
    ABSTRCTATCKTRGTEX_WeakLandForce,
    ABSTRCTATCKTRGTEX_ModerateLandForce,
    ABSTRCTATCKTRGTEX_StrongLandForce,
    ABSTRCTATCKTRGTEX_WeakAirForce,
    ABSTRCTATCKTRGTEX_ModerateAirForce,
    ABSTRCTATCKTRGTEX_StrongAirForce,
    ABSTRCTATCKTRGTEX_END
};

//---------------------------------------------------------------------------
#define Prefix_AbstractPositionEx ABSTRCTPOSEX
enum AbstractPositionEx
{
    ABSTRCTPOSEX_NearBuilding = 1001000, 
	ABSTRCTPOSEX_START        = 1001000,
	ABSTRCTPOSEX_OnAirProtectedBorder, //choose randomly from best protected border buildings 
    ABSTRCTPOSEX_OnNonLandProtectedBorder, //choose randomly from worst protected border buildings
    ABSTRCTPOSEX_OnLandProtectedBorder, //choose randomly from best protected border buildings 
    ABSTRCTPOSEX_OnNonAirProtectedBorder, //choose randomly from worst protected border buildings
    ABSTRCTPOSEX_OnNonAirProtectedBorderInEnemyDirection, //choose randomly from worst protected border buildings InEnemyDirection
    ABSTRCTPOSEX_OnNonLandProtectedBorderInEnemyDirection, //choose randomly from worst protected border buildings InEnemyDirection
    ABSTRCTPOSEX_OnNonProtectedBorderInEnemyDirection, //choose randomly from worst protected border buildings but in enemy direction	
    ABSTRCTPOSEX_OnNonProtectedBorder,
    ABSTRCTPOSEX_OnProtectedBorder,
    ABSTRCTPOSEX_OnBorder, //Choose randomly from border buildings
    ABSTRCTPOSEX_AnyPosition,//Randomly Chosen beside any existing building (except if there's only peon then beside it)

    //Not Computed Yet
    ABSTRCTPOSEX_NearTownCenter, //Near a Random Town Center
    ABSTRCTPOSEX_NearSafestFarm, //Choose randomly from best protected farms
    ABSTRCTPOSEX_NearUnitBuilding, //Choose randomly from all unit buildings
    ABSTRCTPOSEX_NearResourceBuilding, //Choose randomly from all resource buildings
    ABSTRCTPOSEX_NearUnGuardedWoodWorkers, //choose randomly from all unguarded wood workers
    ABSTRCTPOSEX_NearUnGuardedGoldWorkers, //choose randomly from all unguarded gold workers
    ABSTRCTPOSEX_OutOfBorderOppositeEnemyDirection, //Choose randomly from border buildings that are most far from enemy 
    ABSTRCTPOSEX_NearestToAnyPeon,

    //Not Supported Yet
    ABSTRCTPOSEX_NearUnGuardedGoldMine, //Choose randomly from all unguarded gold mines buildings
    ABSTRCTPOSEX_OutOfBorderInEnemyDirection, //Choose randomly from border buildings that are nearest to enemy
    ABSTRCTPOSEX_NearestUnoccupiedGoldMine,
    ABSTRCTPOSEX_END
};

//---------------------------------------------------------------------------
#define Prefix_AbstractPeonEx ABSTRCTPEONEX
enum AbstractPeonEx
{
    ABSTRCTPEONEX_AnyPeon = 1002000,
	ABSTRCTPEONEX_START   = 1002000,
	ABSTRCTPEONEX_IdlePeon,
    ABSTRCTPEONEX_GoldMiner,
    ABSTRCTPEONEX_WoodMiner,
    ABSTRCTPEONEX_NearestPeon,
    ABSTRCTPEONEX_NearestIdlePeon,
    ABSTRCTPEONEX_END
};

//---------------------------------------------------------------------------
#define Prefix_AbstractTrainerEx ABSTRCTTRNEREX
enum AbstractTrainerEx
{
    ABSTRCTTRNEREX_AnyTrainer     = 1003000,
	ABSTRCTTRNEREX_START          = 1003000,
	ABSTRCTTRNEREX_BorderTrainer,
    ABSTRCTTRNEREX_LandProtectedTrainer,
    ABSTRCTTRNEREX_AirProtectedTrainer,
    ABSTRCTTRNEREX_END
};

#define ABSTRACTIONMETADATA_END ABSTRCTTRNEREX_END

#endif // ABSTRACTIONMETADATA_H
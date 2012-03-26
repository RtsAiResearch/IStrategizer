#ifndef RAWMESSAGEDATA_H
#define RAWMESSAGEDATA_H
#include <string>

struct RawLogMessageData
{
	RawLogMessageData()
	{
		GameCycle       = 0;
		UnitNumber      = 0;
		PosX            = 0;
		PosY            = 0;
		DestUnitNumber  = 0;
		PlayerIndex		= 0;
		EnemyIndex		= 0;
	}

	unsigned long   GameCycle;
	int             UnitNumber;
	std::string     UnitIdent;
	std::string     Action;
	int             PosX;
	int             PosY;
	int             PlayerIndex;
	int             EnemyIndex;
	int             DestUnitNumber;
	std::string     Value;
};
//----------------------------------------------------------------------------------------------
struct RawEntityTrainedMessageData
{
	unsigned long   GameCycle;
	int TrainerSlot;
	int EntitySlot;
	std::string TrainerTypeIdent;
	std::string EntityTypeIdent;
	RawEntityTrainedMessageData() : GameCycle(0), TrainerSlot(0), EntitySlot(0) {}
};
//----------------------------------------------------------------------------------------------
struct RawBuildingBuiltMessageData
{
	unsigned long   GameCycle;
	int BuilderSlot;
	int BuildingSlot;
	std::string BuilderTypeIdent;
	std::string BuildingTypeIdent;
	RawBuildingBuiltMessageData() : GameCycle(0), BuilderSlot(0), BuildingSlot(0) {}
};
#endif // RAWMESSAGEDATA_H
#include "MetaData.h"

namespace MetaData
{
	const char* Enums[ENUMS_SIZE] = { NULL };

	void Init()
	{
		static bool initialized = false;

		if (initialized)
			return;

		InitAbstractionMetaData();
		InitTypesMetaData();
		InitAttributesMetaData();

		initialized = true;
	}
	//---------------------------------------------------------------------------
	void InitAbstractionMetaData()
	{

	}
	//----------------------------------------------------------------------------------------------
	void InitTypesMetaData()
	{
		DEFINE(ActionType);
		DEFINE(AttackType);
		DEFINE(BaseType);
		DEFINE(ForceSizeType);
		DEFINE(GoalType);
		DEFINE(ObjectStateType);
		DEFINE(ParameterType);
		DEFINE(PercentageType);
		DEFINE(PlayerType);
		DEFINE(ResourceType);
		DEFINE(StepLevelType);
		DEFINE(StrategyType);
		DEFINE(ExecutionStateType);
	};
	//---------------------------------------------------------------------------
	void InitAttributesMetaData()
	{
		DEFINE(EntityClassAttribute);
		DEFINE(EntityObjectAttribute);
	}
}

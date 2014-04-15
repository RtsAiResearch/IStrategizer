#include "MetaData.h"
#include "SerializationEssentials.h"

namespace IStrategizer
{
    const char* Enums[ENUMS_SIZE] = { nullptr };

    void Init()
    {
        static bool initialized = false;

        if (initialized)
            return;

        SerializationEssentials::Init();
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
        DEFINE(ConditionType);
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

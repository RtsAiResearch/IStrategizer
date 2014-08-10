#include "MetaData.h"

namespace IStrategizer
{
    const char* Enums[ENUMS_SIZE] = { nullptr };

    void InitAttributesMetaData();
    void InitTypesMetaData();

    void IStrategizer::Init()
    {
        static bool initialized = false;

        if (initialized)
            return;

        InitTypesMetaData();
        InitAttributesMetaData();

        initialized = true;
    }
    //----------------------------------------------------------------------------------------------
    void InitTypesMetaData()
    {
        DEFINE(ActionType);
        DEFINE(ConditionType);
        DEFINE(AttackType);
        DEFINE(BaseType);
        DEFINE(GoalType);
        DEFINE(ObjectStateType);
        DEFINE(ParameterType);
        DEFINE(PercentageType);
        DEFINE(PlayerType);
        DEFINE(ResourceType);
        DEFINE(StepLevelType);
        DEFINE(ExecutionStateType);
        DEFINE(DeployType);
		DEFINE(MessageType);
        DEFINE(SituationType);
    };
    //---------------------------------------------------------------------------
    void InitAttributesMetaData()
    {
        DEFINE(EntityTypeProperty);
        DEFINE(EntityObjectProperty);
        DEFINE(PlayerAttribute);
        DEFINE(RtsGameModelAttribute);
    }
}

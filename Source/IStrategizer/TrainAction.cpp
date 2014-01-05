#include "TrainAction.h"

#include <cassert>
#include "Vector2.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "AbstractAdapter.h"
#include "CellFeature.h"
#include "CaseBasedReasonerEx.h"
#include "DataMessage.h"
#include "EngineAssist.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameTechTree.h"
#include "GameType.h"
#include "GameEntity.h"

using namespace IStrategizer;

const unsigned MaxPrepTime = 300000;
const unsigned MaxExecTrialTime = 60000;
const unsigned MaxExecTime = 60000;

TrainAction::TrainAction()
    : Action(ACTIONEX_Train, MaxPrepTime, MaxExecTrialTime, MaxExecTime),
    _trainStarted(false),
    _traineeId(TID()),
    _trainerId(TID())
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
TrainAction::TrainAction(const PlanStepParameters& p_parameters)
    : Action(ACTIONEX_Train, p_parameters, MaxPrepTime, MaxExecTrialTime, MaxExecTime),
    _trainStarted(false),
    _traineeId(TID()),
    _trainerId(TID())
{
}
//----------------------------------------------------------------------------------------------
void TrainAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{
    if (PlanStepEx::State() == ESTATE_Executing && p_pMsg->MessageTypeID() == MSG_EntityCreate)
    {
        EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_pMsg);
        assert(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        TID entityId = pMsg->Data()->EntityId;
        GameEntity *pEntity = g_Game->Self()->GetEntity(entityId);
        assert(pEntity);

        if (pEntity->Type() == _params[PARAM_EntityClassId])
        {
            // Check if the trainer is training that entity
            GameEntity* pTrainer = g_Game->Self()->GetEntity(_trainerId);
            assert(pTrainer);

            if (pTrainer->IsTraining(entityId))
            {
                _trainStarted = true;
                _traineeId = entityId;
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
bool TrainAction::PreconditionsSatisfied()
{
    EntityClassType trainerType;
    EntityClassType traineeType = (EntityClassType)_params[PARAM_EntityClassId];
    bool success = false;

    // Condition #1: make sure that the source building exists
    trainerType = g_Game->Self()->TechTree()->SourceEntity(traineeType);
    success = g_Assist.DoesEntityClassExist(make_pair(trainerType, 1));

    if (!success)
        return false;

    // Condition #2: assure that we have enough resources
    g_Assist.PrerequisitesSatisfied(traineeType, success);

    return success;
}
//----------------------------------------------------------------------------------------------
bool TrainAction::AliveConditionsSatisfied()
{
    bool trainerExist = false;
    bool traineeExist = false;
    bool trainerBusy = false;
    bool traineeBeingTrained = false;
    bool success = false;

    // 1. Trainer building exist
    trainerExist = g_Assist.DoesEntityObjectExist(_trainerId);

    if (trainerExist)
    {
        if (_trainStarted)
        {
            // 2. Trainer building is busy or in the training state
            GameEntity* pTrainer = g_Game->Self()->GetEntity(_trainerId);
            assert(pTrainer);
            ObjectStateType trainerState = (ObjectStateType)pTrainer->Attr(EOATTR_State);
            trainerBusy = trainerState == OBJSTATE_Training;

            if (trainerBusy)
            {
                // 3. The trainee unit object exist, i.e not cancel
                traineeExist = g_Assist.DoesEntityObjectExist(_traineeId);

                if (traineeExist)
                {
                    // 4. Trainee is still being trained
                    GameEntity* pTrainee = g_Game->Self()->GetEntity(_traineeId);
                    assert(pTrainee);
                    ObjectStateType traineeState = (ObjectStateType)pTrainee->Attr(EOATTR_State);
                    traineeBeingTrained = traineeState == OBJSTATE_BeingConstructed;

                    if (traineeBeingTrained)
                        success = true;
                }
            }
        }
        else
        {
            success = true;
        }
    }

    return success;
}
//----------------------------------------------------------------------------------------------
bool TrainAction::SuccessConditionsSatisfied()
{
    bool success = false;
    bool traineeBeingTrained = false;

    if (_trainStarted)
    {
        // 1. Trainee unit object exist
        bool traineeExist = g_Assist.DoesEntityObjectExist(_traineeId);

        if (traineeExist)
        {
            // 2. Trainee is ready and no more being constructed
            GameEntity* pTrainee = g_Game->Self()->GetEntity(_traineeId);
            assert(pTrainee);
            ObjectStateType traineeState = (ObjectStateType)pTrainee->Attr(EOATTR_State);
            traineeBeingTrained = traineeState == OBJSTATE_BeingConstructed;

            if (!traineeBeingTrained)
                success = true;
        }
    }

    return success;
}
//----------------------------------------------------------------------------------------------
bool TrainAction::ExecuteAux(const WorldClock& p_clock)
{
    EntityClassType traineeType = (EntityClassType)_params[PARAM_EntityClassId];
    GameEntity *pGameTrainer;
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool executed = false;

    // Adapt trainer
    _trainerId = pAdapter->AdaptBuildingForTraining(traineeType);

    if (_trainerId != INVALID_TID)
    {
        // Issue train order
        pGameTrainer = g_Game->Self()->GetEntity(_trainerId);
        assert(pGameTrainer);
        executed = pGameTrainer->Train(traineeType);
    }

    return executed;
}

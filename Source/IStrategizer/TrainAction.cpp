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
#include "EntityClassExist.h"

using namespace IStrategizer;

const unsigned MaxPrepTime = 0;
const unsigned MaxExecTrialTime = 60000;
const unsigned MaxExecTime = 0;

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
    _trainerId(TID()),
    m_pTrainee(nullptr)
{
}
//----------------------------------------------------------------------------------------------
void TrainAction::HandleMessage(RtsGame& pRtsGame, Message* p_msg, bool& p_consumed)
{
    if (PlanStepEx::State() == ESTATE_Executing && p_msg->MessageTypeID() == MSG_EntityCreate)
    {
        EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_msg);
        _ASSERTE(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        TID entityId = pMsg->Data()->EntityId;
        GameEntity *pEntity = pRtsGame.Self()->GetEntity(entityId);
        _ASSERTE(pEntity);

        // We are interested only in free trainees that have not been locked before
        if (pEntity->Type() == _params[PARAM_EntityClassId] &&
            !pEntity->IsLocked())
        {
            // Check if the trainer is training that entity
            GameEntity* pTrainer = pRtsGame.Self()->GetEntity(_trainerId);
            _ASSERTE(pTrainer);

            if (pTrainer->IsTraining(entityId))
            {
                _trainStarted = true;
                _traineeId = entityId;

                m_pTrainee = pEntity;

                // Lock that trainee and bound it to this action because if we don't
                // other ready actions in the same update cycle will receive the same message
                // and they may bind to the same trainee
                pEntity->Lock(this);

                LogInfo("Action %s has bound trainee=%d to trainer=%d", ToString().c_str(), _traineeId, _trainerId);
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
bool TrainAction::AliveConditionsSatisfied(RtsGame& pRtsGame)
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
            GameEntity* pTrainer = pRtsGame.Self()->GetEntity(_trainerId);
            _ASSERTE(pTrainer);
            ObjectStateType trainerState = (ObjectStateType)pTrainer->Attr(EOATTR_State);
            trainerBusy = trainerState == OBJSTATE_Training;
            // 3. The trainee unit object exist, i.e not cancel
            traineeExist = g_Assist.DoesEntityObjectExist(_traineeId);
            
            if (traineeExist && !trainerBusy)
            {
                success = true;
            }
            else if (trainerBusy)
            {
                if (traineeExist)
                {
                    // 4. Trainee is still being trained
                    GameEntity* pTrainee = pRtsGame.Self()->GetEntity(_traineeId);
                    _ASSERTE(pTrainee);
                    ObjectStateType traineeState = (ObjectStateType)pTrainee->Attr(EOATTR_State);
                    traineeBeingTrained = traineeState == OBJSTATE_BeingConstructed;

                    if (traineeBeingTrained || traineeState == OBJSTATE_Idle)
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
bool TrainAction::SuccessConditionsSatisfied(RtsGame& pRtsGame)
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
            GameEntity* pTrainee = pRtsGame.Self()->GetEntity(_traineeId);
            _ASSERTE(pTrainee);
            ObjectStateType traineeState = (ObjectStateType)pTrainee->Attr(EOATTR_State);
            traineeBeingTrained = traineeState == OBJSTATE_BeingConstructed;

            if (!traineeBeingTrained)
            {
                LogInfo("Action %s succeeded to train trainee=%d from trainer=%d", ToString().c_str(), _traineeId, _trainerId);
                success = true;
            }
        }
    }

    return success;
}
//----------------------------------------------------------------------------------------------
bool TrainAction::ExecuteAux(RtsGame& pRtsGame, const WorldClock& p_clock)
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
        pGameTrainer = pRtsGame.Self()->GetEntity(_trainerId);
        _ASSERTE(pGameTrainer);
        executed = pGameTrainer->Train(traineeType);

        if (executed)
            LogInfo("Action %s commanded trainer=%d to train trainee=%d", ToString().c_str(), _trainerId, _traineeId);
    }

    return executed;
}
//----------------------------------------------------------------------------------------------
void TrainAction::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], 1, true));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void TrainAction::InitializePreConditions()
{
    EntityClassType traineeType = (EntityClassType)_params[PARAM_EntityClassId];
    EntityClassType trainerType = g_Game->Self()->TechTree()->SourceEntity(traineeType);
    vector<Expression*> m_terms;

    m_terms.push_back(new EntityClassExist(PLAYER_Self, trainerType, 1, true));
    g_Assist.GetPrerequisites(traineeType, PLAYER_Self, m_terms);
    _preCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void IStrategizer::TrainAction::OnSucccess(RtsGame& pRtsGame, const WorldClock& p_clock)
{
    if (m_pTrainee != nullptr)
        m_pTrainee->Unlock(this);
}
//----------------------------------------------------------------------------------------------
void IStrategizer::TrainAction::OnFailure(RtsGame& pRtsGame, const WorldClock& p_clock)
{
    if (m_pTrainee != nullptr)
        m_pTrainee->Unlock(this);
}

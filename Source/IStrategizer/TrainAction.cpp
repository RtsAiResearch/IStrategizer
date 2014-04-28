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
    m_trainStarted(false),
    m_traineeId(TID()),
    m_trainerId(TID()),
    m_pTrainee(nullptr)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
TrainAction::TrainAction(const PlanStepParameters& params)
    : Action(ACTIONEX_Train, params, MaxPrepTime, MaxExecTrialTime, MaxExecTime),
    m_trainStarted(false),
    m_traineeId(TID()),
    m_trainerId(TID()),
    m_pTrainee(nullptr)
{
}
//----------------------------------------------------------------------------------------------
void TrainAction::HandleMessage(RtsGame& game, Message* pMsg, bool& consumed)
{
    if (PlanStepEx::State() == ESTATE_Executing && pMsg->MessageTypeID() == MSG_EntityCreate)
    {
        EntityCreateMessage* pEntityMsg = static_cast<EntityCreateMessage*>(pMsg);
        _ASSERTE(pEntityMsg && pEntityMsg->Data());

        if (pEntityMsg->Data()->OwnerId != PLAYER_Self)
            return;

        TID entityId = pEntityMsg->Data()->EntityId;
        GameEntity *pEntity = game.Self()->GetEntity(entityId);
        _ASSERTE(pEntity);

        // We are interested only in free trainees that have not been locked before
        if (pEntity->Type() == _params[PARAM_EntityClassId] &&
            !pEntity->IsLocked())
        {
            // Check if the trainer is training that entity
            GameEntity* pTrainer = game.Self()->GetEntity(m_trainerId);
            _ASSERTE(pTrainer);

            if (pTrainer->IsTraining(entityId))
            {
                m_trainStarted = true;
                m_traineeId = entityId;

                m_pTrainee = pEntity;

                // Lock that trainee and bound it to this action because if we don't
                // other ready actions in the same update cycle will receive the same message
                // and they may bind to the same trainee
                pEntity->Lock(this);

                LogInfo("Action %s has bound trainee=%d to trainer=%d", ToString().c_str(), m_traineeId, m_trainerId);
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
bool TrainAction::AliveConditionsSatisfied(RtsGame& game)
{
    bool trainerExist = false;
    bool traineeExist = false;
    bool trainerBusy = false;
    bool traineeBeingTrained = false;
    bool success = false;

    // 1. Trainer building exist
    trainerExist = g_Assist.DoesEntityObjectExist(m_trainerId);

    if (trainerExist)
    {
        if (m_trainStarted)
        {
            // 2. Trainer building is busy or in the training state
            GameEntity* pTrainer = game.Self()->GetEntity(m_trainerId);
            _ASSERTE(pTrainer);
            ObjectStateType trainerState = (ObjectStateType)pTrainer->Attr(EOATTR_State);
            trainerBusy = trainerState == OBJSTATE_Training;
            // 3. The trainee unit object exist, i.e not cancel
            traineeExist = g_Assist.DoesEntityObjectExist(m_traineeId);
            
            if (traineeExist && !trainerBusy)
            {
                success = true;
            }
            else if (trainerBusy)
            {
                if (traineeExist)
                {
                    // 4. Trainee is still being trained
                    GameEntity* pTrainee = game.Self()->GetEntity(m_traineeId);
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
    else
    {
        ConditionEx* failedCondition = new EntityClassExist(PLAYER_Self, m_trainerType, 1, false);
        m_history.Add(ESTATE_Failed, failedCondition);
    }

    return success;
}
//----------------------------------------------------------------------------------------------
bool TrainAction::SuccessConditionsSatisfied(RtsGame& game)
{
    bool success = false;
    bool traineeBeingTrained = false;

    if (m_trainStarted)
    {
        // 1. Trainee unit object exist
        bool traineeExist = g_Assist.DoesEntityObjectExist(m_traineeId);

        if (traineeExist)
        {
            // 2. Trainee is ready and no more being constructed
            GameEntity* pTrainee = game.Self()->GetEntity(m_traineeId);
            _ASSERTE(pTrainee);
            ObjectStateType traineeState = (ObjectStateType)pTrainee->Attr(EOATTR_State);
            traineeBeingTrained = traineeState == OBJSTATE_BeingConstructed;

            if (!traineeBeingTrained)
            {
                LogInfo("Action %s succeeded to train trainee=%d from trainer=%d", ToString().c_str(), m_traineeId, m_trainerId);
                success = true;
            }
        }
    }

    return success;
}
//----------------------------------------------------------------------------------------------
bool TrainAction::ExecuteAux(RtsGame& game, const WorldClock& clock)
{
    EntityClassType traineeType = (EntityClassType)_params[PARAM_EntityClassId];
    GameEntity *pGameTrainer;
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool executed = false;

    // Adapt trainer
    m_trainerId = pAdapter->AdaptBuildingForTraining(traineeType);

    if (m_trainerId != INVALID_TID)
    {
        // Issue train order
        pGameTrainer = game.Self()->GetEntity(m_trainerId);
        _ASSERTE(pGameTrainer);
        executed = pGameTrainer->Train(traineeType);

        if (executed)
        {
            _ASSERTE(!m_requiredResources.IsNull());
            m_requiredResources.Lock(this);

            LogInfo("Action %s commanded trainer=%d to train trainee=%d", ToString().c_str(), m_trainerId, m_traineeId);
        }
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
    m_trainerType = g_Game->Self()->TechTree()->SourceEntity(traineeType);
    vector<Expression*> m_terms;
    WorldResources completeRequiredRespurces = WorldResources::FromEntity(traineeType);
    
    // Do not lock resources other than supply, because supply does not get consumed
    // when the action is triggered. Unlike primary and secondary resources which get
    // consumed upon executing the action
    m_requiredResources = WorldResources(completeRequiredRespurces.Supply(), 0, 0);

    m_terms.push_back(new EntityClassExist(PLAYER_Self, m_trainerType, 1, true));
    g_Assist.GetPrerequisites(traineeType, PLAYER_Self, m_terms);
    _preCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void IStrategizer::TrainAction::OnSucccess(RtsGame& game, const WorldClock& clock)
{
    GameEntity* pTrainee = game.Self()->GetEntity(m_traineeId);

    if (pTrainee && pTrainee->IsLocked() && pTrainee->Owner() == this)
        pTrainee->Unlock(this);

    _ASSERTE(!m_requiredResources.IsNull());
    if (m_requiredResources.IsLocked())
        m_requiredResources.Unlock(this);
}
//----------------------------------------------------------------------------------------------
void IStrategizer::TrainAction::OnFailure(RtsGame& game, const WorldClock& clock)
{
    GameEntity* pTrainee = game.Self()->GetEntity(m_traineeId);

    if (pTrainee && pTrainee->IsLocked() && pTrainee->Owner() == this)
        pTrainee->Unlock(this);

    _ASSERTE(!m_requiredResources.IsNull());
    if (m_requiredResources.IsLocked())
        m_requiredResources.Unlock(this);
}

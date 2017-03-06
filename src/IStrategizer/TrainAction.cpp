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
#include "PlayerAttributeExist.h"
#include "Logger.h"
#include "TrainerExist.h"
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace std;

DECL_SERIALIZABLE(TrainAction);

const unsigned MaxPrepTime = 0;
// MaxExecTime should be deduced from the unit being trained
const unsigned MaxExecTime = 0;

TrainAction::TrainAction()
: Action(ACTIONEX_Train, MaxPrepTime, MaxExecTime),
m_traineeId(INVALID_TID),
m_trainerId(INVALID_TID)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    CellFeature::Null().To(_params);
}
//////////////////////////////////////////////////////////////////////////
TrainAction::TrainAction(const PlanStepParameters& params)
: Action(ACTIONEX_Train, params, MaxPrepTime, MaxExecTime),
m_traineeId(INVALID_TID),
m_trainerId(INVALID_TID)
{
}
//////////////////////////////////////////////////////////////////////////
void TrainAction::HandleMessage(Message* pMsg, bool& consumed)
{
    if (PlanStepEx::GetState() == ESTATE_Executing && pMsg->TypeId() == MSG_EntityCreate)
    {
        EntityCreateMessage* pEntityMsg = static_cast<EntityCreateMessage*>(pMsg);
        _ASSERTE(pEntityMsg && pEntityMsg->Data());

        if (pEntityMsg->Data()->OwnerId != PLAYER_Self)
            return;

        TID entityId = pEntityMsg->Data()->EntityId;
        GameEntity *pEntity = g_Game->Self()->GetEntity(entityId);
        _ASSERTE(pEntity);

        // We are interested only in free trainees that have not been locked before
        if (m_traineeId == INVALID_TID &&
            pEntity->TypeId() == _params[PARAM_EntityClassId] &&
            !pEntity->IsLocked())
        {
            // Check if the trainer is training that entity
            GameEntity* pTrainer = g_Game->Self()->GetEntity(m_trainerId);
            _ASSERTE(pTrainer);

            if (pTrainer->IsTraining(entityId))
            {
                m_traineeId = entityId;

                // Lock that trainee and bound it to this action because if we don't
                // other ready actions in the same update cycle will receive the same message
                // and they may bind to the same trainee
                pEntity->Lock(this);
                consumed = true;
                LogInfo("Action %s has bound trainee=%d to trainer=%d", ToString().c_str(), m_traineeId, m_trainerId);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool TrainAction::AliveConditionsSatisfied()
{
    bool trainerExist = false;
    //bool traineeExist = false;
    //bool trainerBusy = false;
    //bool traineeBeingTrained = false;
    bool success = false;

    // 1. Trainer building exist
    trainerExist = g_Assist.DoesEntityObjectExist(m_trainerId);

    if (trainerExist)
    {
        // 2. Trainer building is busy or in the training state
        GameEntity* pTrainer = g_Game->Self()->GetEntity(m_trainerId);

        if (pTrainer->P(OP_IsTraining))
        {
            success = true;

            //if (m_traineeId != INVALID_TID)
            //{
            //    // 3. The trainee unit object exist, i.e not cancel
            //    traineeExist = g_Assist.DoesEntityObjectExist(m_traineeId);

            //    if (traineeExist && !trainerBusy)
            //    {
            //        success = true;
            //    }
            //    else if (trainerBusy)
            //    {
            //        if (traineeExist)
            //        {
            //            // 4. Trainee is still being trained
            //            GameEntity* pTrainee = game.Self()->GetEntity(m_traineeId);
            //            _ASSERTE(pTrainee);
            //            ObjectStateType traineeState = (ObjectStateType)pTrainee->P(OP_State);
            //            traineeBeingTrained = traineeState == OBJSTATE_BeingConstructed;

            //            if (traineeBeingTrained || traineeState == OBJSTATE_Idle)
            //                success = true;
            //        }
            //    }
            //}
        }
    }

    if (!success)
    {
        ConditionEx* failedCondition = new EntityClassExist(PLAYER_Self, m_trainerType, 1);
        m_history.Add(ESTATE_Failed, failedCondition);
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
bool TrainAction::SuccessConditionsSatisfied(RtsGame& game)
{
    bool success = false;
    bool traineeBeingTrained = false;

    if (m_traineeId != INVALID_TID)
    {
        // 1. Trainee unit object exist
        bool traineeExist = g_Assist.DoesEntityObjectExist(m_traineeId);

        if (traineeExist)
        {
            // 2. Trainee is ready and no more being constructed
            GameEntity* pTrainee = game.Self()->GetEntity(m_traineeId);
            _ASSERTE(pTrainee);
            ObjectStateType traineeState = (ObjectStateType)pTrainee->P(OP_State);
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
//////////////////////////////////////////////////////////////////////////
bool TrainAction::Execute()
{
    LogActivity(Execute);

    EntityClassType traineeType = (EntityClassType)_params[PARAM_EntityClassId];
    GameEntity *pGameTrainer;
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool executed = false;

    // Adapt trainer
    m_trainerId = pAdapter->AdaptBuildingForTraining(traineeType);

    if (m_trainerId != INVALID_TID)
    {
        // Issue train order
        pGameTrainer = g_Game->Self()->GetEntity(m_trainerId);
        _ASSERTE(pGameTrainer);
        executed = pGameTrainer->Train(traineeType);

        if (executed)
        {
            pGameTrainer->Lock(this);
        }
    }

    return executed;
}
//////////////////////////////////////////////////////////////////////////
void TrainAction::InitializePostConditions()
{
    vector<Expression*> m_terms;
    EntityClassType entityTypeId = (EntityClassType)_params[PARAM_EntityClassId];
    GameType *pGameType = g_Game->GetEntityType(entityTypeId);

    if (!pGameType->P(TP_IsWorker))
    {
        m_terms.push_back(new PlayerAttributeExist(PLAYER_Self, PATTR_AlliedAttackersTotalHP, g_Game->GetEntityType(entityTypeId)->P(TP_MaxHp)));
        m_terms.push_back(new PlayerAttributeExist(PLAYER_Self, PATTR_AlliedAttackersTotalDamage, g_Game->GetEntityType(entityTypeId)->P(TP_GroundAttack)));
    }

    m_terms.push_back(new EntityClassExist(PLAYER_Self, entityTypeId, 1));
    _postCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
void TrainAction::InitializePreConditions()
{
    EntityClassType traineeType = (EntityClassType)_params[PARAM_EntityClassId];
    m_trainerType = g_Game->GetEntityType(traineeType)->SourceEntity();
    vector<Expression*> m_terms;

    m_terms.push_back(new TrainerExist(traineeType));
    g_Assist.GetPrerequisites(traineeType, PLAYER_Self, m_terms);
    _preCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
void TrainAction::FreeResources()
{
    if (m_traineeId != INVALID_TID)
    {
        GameEntity* pTrainee = g_Game->Self()->GetEntity(m_traineeId);

        if (pTrainee && pTrainee->IsLocked())
            pTrainee->Unlock(this);

        m_traineeId = INVALID_TID;
    }

    if (m_trainerId != INVALID_TID)
    {
        GameEntity* pTrainer = g_Game->Self()->GetEntity(m_trainerId);

        if (pTrainer && pTrainer->IsLocked())
            pTrainer->Unlock(this);

        m_trainerId = INVALID_TID;
    }
}
//////////////////////////////////////////////////////////////////////////
bool TrainAction::Equals(PlanStepEx* p_planStep)
{
    return StepTypeId() == p_planStep->StepTypeId() &&
        _params[PARAM_EntityClassId] == p_planStep->Parameter(PARAM_EntityClassId);
}

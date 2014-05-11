#include "BuildActionEx.h"

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
#include "AdapterEx.h"
#include "EntityClassExist.h"
#include "PlayerResources.h"

using namespace IStrategizer;

const unsigned MaxPrepTime = 300000;
const unsigned MaxExecTrialTime = 120000;
const unsigned MaxExecTime = 120000;

BuildActionEx::BuildActionEx() :
    Action(ACTIONEX_Build, MaxPrepTime, MaxExecTrialTime, MaxExecTime), _buildStarted(false), _buildIssued(false)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    CellFeature::Null().To(_params);
}
//////////////////////////////////////////////////////////////////////////
BuildActionEx::BuildActionEx(const PlanStepParameters& p_parameters) :
    Action(ACTIONEX_Build, p_parameters, MaxPrepTime, MaxExecTrialTime, MaxExecTime), _buildStarted(false), _buildIssued(false)
{
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::OnSucccess(RtsGame& game, const WorldClock& p_clock)
{
    FreeResources(game);
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::OnFailure(RtsGame& game, const WorldClock& p_clock)
{
    FreeResources(game);
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::FreeResources(RtsGame &game)
{
    if (!_buildArea.IsNull() && _buildArea.IsLocked())
    {
        // Special buildings (for example addons) are not associated with build positions so no need to assert in that case.
        _ASSERTE(game.Self()->IsSpecialBuilding((EntityClassType)_params[PARAM_EntityClassId]) || !_buildArea.IsNull());
        _buildArea.Unlock(this);
    }

    if (!_requiredResources.IsNull() && _requiredResources.IsLocked())
    {
        _requiredResources.Unlock(this);
    }

    if (_builderId != INVALID_TID)
    {
        GameEntity *pEntity = game.Self()->GetEntity(_builderId);

        if (pEntity && pEntity->IsLocked())
        {
            pEntity->Unlock(this);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
{
    if(PlanStepEx::State() == ESTATE_Executing &&
        (p_msg->MessageTypeID() == MSG_EntityCreate || 
        p_msg->MessageTypeID() == MSG_EntityRenegade))
    {
        EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_msg);
        TID buildingId;
        GameEntity *pGameBuilding;
        Vector2 msgBuildPosition;

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        _ASSERTE(pMsg && pMsg->Data());
        buildingId = pMsg->Data()->EntityId;

        pGameBuilding = game.Self()->GetEntity(buildingId);
        _ASSERTE(pGameBuilding);

        msgBuildPosition.X = pMsg->Data()->X;
        msgBuildPosition.Y = pMsg->Data()->Y;

        if (pGameBuilding->Type() == _params[PARAM_EntityClassId] &&
            ((msgBuildPosition.X == _buildArea.Pos().X && msgBuildPosition.Y == _buildArea.Pos().Y) ||
            game.Self()->IsSpecialBuilding(pGameBuilding->Type())))
        {
            _buildingId = pGameBuilding->Id();
            _buildStarted = true;
            _ASSERTE(!_requiredResources.IsNull());
            _requiredResources.Unlock(this);
            p_consumed = true;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::AliveConditionsSatisfied(RtsGame& game)
{
    bool builderExist = false;
    bool buildingExist = false;
    bool isBuilderConstructing = false;
    bool success = false;
    GameEntity *pEntity = nullptr;

    _ASSERTE(PlanStepEx::State() == ESTATE_Executing);

    builderExist = g_Assist.DoesEntityObjectExist(_builderId);

    if (builderExist)
    {
        success = true;
        pEntity = game.Self()->GetEntity(_builderId);

        _ASSERTE(pEntity);
        isBuilderConstructing = (pEntity->Attr(EOATTR_State) == OBJSTATE_Constructing);

        if (!isBuilderConstructing)
        {
            LogInfo("Builder with ID=%d of action %s is not in the constructing state", _builderId, ToString().c_str());

            if (!(pEntity->Attr(EOATTR_IsMoving) > 0 ? true : false) && !_buildStarted)
            {
                LogInfo("Builder with ID=%d of action %s is not moving toward the build position, will NOT fail the action but this is weird case.", _builderId, ToString().c_str());
            }
        }
        else
        {
            if (_buildStarted)
            {
                buildingExist = g_Assist.DoesEntityObjectExist(_buildingId);

                if (!buildingExist)
                {
                    success = false;
                }
            }
        }
    }
    else
    {
        ConditionEx* failedCondition = new EntityClassExist(
            PLAYER_Self,
            game.Self()->GetWorkerType(),
            1,
            true);
        m_history.Add(ESTATE_Failed, failedCondition);

        LogInfo("Builder with ID=%d of action %s does not exist", _builderId, ToString().c_str());
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::SuccessConditionsSatisfied(RtsGame& game)
{
    _ASSERTE(PlanStepEx::State() == ESTATE_Executing);

    if (_buildStarted)
    {
        int entityState;
        GameEntity *pEntity;

        pEntity = game.Self()->GetEntity(_buildingId);    
        entityState = pEntity->Attr(EOATTR_State);

        return entityState != OBJSTATE_BeingConstructed;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::ExecuteAux(RtsGame& game, const WorldClock& p_clock)
{
    EntityClassType buildingType = (EntityClassType)_params[PARAM_EntityClassId];
    GameEntity *pGameBuilder;
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool bOk = false;

    // Adapt builder
    _builderId = pAdapter->GetEntityObjectId(game.Self()->GetBuilderType(buildingType), AdapterEx::WorkerStatesRankVector);

    if (_builderId != INVALID_TID)
    {
        // Initialize build state
        _buildStarted = false;

        // Adapt build position
        _ASSERTE(pAdapter);
        _buildArea = pAdapter->AdaptPositionForBuilding(buildingType);

        // Issue build order
        pGameBuilder = game.Self()->GetEntity(_builderId);
        _ASSERTE(pGameBuilder);

        LogInfo("Builder=%d was selected to execute build", _builderId);

        pGameBuilder->Lock(this);
        // Special buildings (for example addons) are not associated with build positions so no need to assert in that case.
        _ASSERTE(game.Self()->IsSpecialBuilding(buildingType) || !_buildArea.IsNull());
        _buildArea.Lock(this);
        _ASSERTE(!_requiredResources.IsNull());
        _requiredResources.Lock(this);
        bOk = pGameBuilder->Build(buildingType, _buildArea.Pos());

        if (bOk)
            _buildIssued = true;
    }

    return bOk;
}
//----------------------------------------------------------------------------------------------
void BuildActionEx::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], 1));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void BuildActionEx::InitializePreConditions()
{
    EntityClassType builderType = g_Game->Self()->GetWorkerType();
    EntityClassType buildingType = (EntityClassType)_params[PARAM_EntityClassId];
    _requiredResources = WorldResources::FromEntity(buildingType);
    vector<Expression*> m_terms;

    m_terms.push_back(new EntityClassExist(PLAYER_Self, builderType, 1, true));
    g_Assist.GetPrerequisites(buildingType, PLAYER_Self, m_terms);
    _preCondition = new And(m_terms);
}
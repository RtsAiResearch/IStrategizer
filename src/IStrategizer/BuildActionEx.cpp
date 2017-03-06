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
#include "ResourceExist.h"
#include "PlayerResources.h"
#include "Logger.h"
#include "BuilderExist.h"
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace std;

DECL_SERIALIZABLE(BuildActionEx);

const unsigned MaxPrepTime = 0;
// In BuildAction case, the MaxExecTime is the time between issuing the Build command
// and the worker starts actually build the building
const unsigned MaxExecTime = 0;

BuildActionEx::BuildActionEx() :
Action(ACTIONEX_Build, MaxPrepTime, MaxExecTime), _buildStarted(false), _buildIssued(false)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    CellFeature::Null().To(_params);
}
//////////////////////////////////////////////////////////////////////////
BuildActionEx::BuildActionEx(const PlanStepParameters& p_parameters) :
Action(ACTIONEX_Build, p_parameters, MaxPrepTime, MaxExecTime), _buildStarted(false), _buildIssued(false)
{
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::FreeResources()
{
    if (_builderId != INVALID_TID)
    {
        if (!_buildArea.IsNull() && _buildArea.IsLocked())
        {
            // Special buildings (for example addons) are not associated with build positions so no need to assert in that case.
            _ASSERTE(g_Game->GetEntityType((EntityClassType)_params[PARAM_EntityClassId])->P(TP_IsSpecialBuilding) || !_buildArea.IsNull());
            _buildArea.Unlock(this);
        }

        if (!_requiredResources.IsNull() && _requiredResources.IsLocked())
        {
            _requiredResources.Unlock(this);
        }

        if (_builderId != INVALID_TID)
        {
            GameEntity *pEntity = g_Game->Self()->GetEntity(_builderId);

            if (pEntity && pEntity->IsLocked())
            {
                pEntity->Unlock(this);
            }

            _builderId = INVALID_TID;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::HandleMessage(Message* p_msg, bool& p_consumed)
{
    if (PlanStepEx::GetState() == ESTATE_Executing &&
        (p_msg->TypeId() == MSG_EntityCreate ||
        p_msg->TypeId() == MSG_EntityRenegade))
    {
        EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_msg);
        TID buildingId;
        GameEntity *pGameBuilding;
        Vector2 msgBuildPosition;

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        _ASSERTE(pMsg && pMsg->Data());
        buildingId = pMsg->Data()->EntityId;

        pGameBuilding = g_Game->Self()->GetEntity(buildingId);
        _ASSERTE(pGameBuilding);

        msgBuildPosition.X = pMsg->Data()->X;
        msgBuildPosition.Y = pMsg->Data()->Y;

        if (pGameBuilding->TypeId() == _params[PARAM_EntityClassId] &&
            ((msgBuildPosition.X == _buildArea.Pos().X && msgBuildPosition.Y == _buildArea.Pos().Y) ||
            g_Game->GetEntityType(pGameBuilding->TypeId())->P(TP_IsSpecialBuilding)))
        {
            _buildingId = pGameBuilding->Id();
            _buildStarted = true;
            _ASSERTE(!_requiredResources.IsNull());
            _requiredResources.Unlock(this);
            p_consumed = true;
            LogInfo("%s started actual building of %s", ToString().c_str(), pGameBuilding->ToString().c_str());
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::AliveConditionsSatisfied()
{
    bool builderExist = false;
    bool buildingExist = false;
    bool success = false;
    GameEntity *pEntity = nullptr;

    _ASSERTE(PlanStepEx::GetState() == ESTATE_Executing);

    builderExist = g_Assist.DoesEntityObjectExist(_builderId);

    if (builderExist)
    {
        success = true;
        pEntity = g_Game->Self()->GetEntity(_builderId);

        _ASSERTE(pEntity);
        auto pBuilderType = g_Game->GetEntityType(pEntity->TypeId());


        if (pBuilderType->P((TP_IsBuilding)))
        {
            LogDebugInfo("Checking state of builder %s", pEntity->ToString().c_str());
            ObjectStateType state = (ObjectStateType)pEntity->P(OP_State);

            if (state != OBJSTATE_Constructing)
            {
                LogInfo("Builder %s of action %s is not constructing an expansion, something wrong happened, failing the build", pEntity->ToString().c_str(), ToString().c_str());
                success = false;
            }
        }
        // For workers, check if the worker is stuck, otherwise 
        // If a building is building another building, then no need to check
        // for stucking
        else
        {
            if (pEntity->P(OP_State) == OBJSTATE_Idle)
            {
                LogInfo("Builder %s of action %s is standing idle, something wrong happened, failing the build", pEntity->ToString().c_str(), ToString().c_str());
                success = false;
            }
            else if (pEntity->P(OP_State) == OBJSTATE_Constructing)
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
            else if (pEntity->P(OP_State) == OBJSTATE_GatheringPrimary ||
                pEntity->P(OP_State) == OBJSTATE_GatheringSecondary)
            {
                LogInfo("Builder %s of action %s is gathering, something wrong happened, failing the build", pEntity->ToString().c_str(), ToString().c_str());
                success = false;
            }
            else
            {
                LogInfo("Builder %s of action %s is not in the constructing state, didn't start actual constructing yet", pEntity->ToString().c_str(), ToString().c_str());
            }
        }
    }
    else
    {
        ConditionEx* failedCondition = new EntityClassExist(
            PLAYER_Self,
            g_Game->Self()->Race()->GetWorkerType(),
            1);
        m_history.Add(ESTATE_Failed, failedCondition);

        LogInfo("Builder with ID=%d of action %s does not exist", _builderId, ToString().c_str());
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::SuccessConditionsSatisfied(RtsGame& game)
{
    _ASSERTE(PlanStepEx::GetState() == ESTATE_Executing);

    if (_buildStarted)
    {
        GameEntity *pEntity = game.Self()->GetEntity(_buildingId);

        if (pEntity)
        {
            int entityState = pEntity->P(OP_State);
            return entityState != OBJSTATE_BeingConstructed;
        }
        else
            return false;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::Execute()
{
    EntityClassType buildingType = (EntityClassType)_params[PARAM_EntityClassId];
    GameEntity *pGameBuilder;
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool bOk = false;

    //// Adapt builder
    //_builderId = pAdapter->AdaptBuilder(buildingType, true);
    //// Adapt build position
    //_buildArea = pAdapter->AdaptPositionForBuilding(buildingType);

    // Adapt builder
    // Adapt build position
    auto adaptedParams = pAdapter->AdaptBuilderAndPosition(buildingType, true);
    _builderId = adaptedParams.first;
    _buildArea = adaptedParams.second;

    if (_builderId != INVALID_TID)
    {
        // Initialize build state
        _buildStarted = false;

        // Issue build order
        pGameBuilder = g_Game->Self()->GetEntity(_builderId);

        LogInfo("Builder=%s was selected to execute build", pGameBuilder->ToString().c_str());

        pGameBuilder->Lock(this);

        // Special buildings (for example addons) are not associated with build positions so no need to assert in that case.
        if (!g_Game->GetEntityType(buildingType)->P(TP_IsSpecialBuilding))
        {
            _ASSERTE(!_buildArea.IsNull());
            _buildArea.Lock(this);
        }

        _ASSERTE(!_requiredResources.IsNull());
        _requiredResources.Lock(this);
        bOk = pGameBuilder->Build(buildingType, _buildArea.Pos());

        if (bOk)
            _buildIssued = true;
    }

    return bOk;
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId]));

    if (g_Game->Self()->Race()->GetResourceSource(RESOURCE_Supply) == (EntityClassType)_params[PARAM_EntityClassId])
    {
        m_terms.push_back(new ResourceExist(PLAYER_Self, RESOURCE_Supply, g_Game->Self()->Race()->SupplyBuildingSupplyAmount()));
    }

    _postCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::InitializePreConditions()
{
    EntityClassType buildingType = (EntityClassType)_params[PARAM_EntityClassId];
    _requiredResources = WorldResources::FromEntity(buildingType);
    vector<Expression*> m_terms;

    m_terms.push_back(new BuilderExist(buildingType));

    g_Assist.GetPrerequisites(buildingType, PLAYER_Self, m_terms);
    _preCondition = new And(m_terms);
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::Equals(PlanStepEx* p_planStep)
{
    return StepTypeId() == p_planStep->StepTypeId() &&
        _params[PARAM_EntityClassId] == p_planStep->Parameter(PARAM_EntityClassId);
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::Abort()
{
    if (_builderId != INVALID_TID)
    {
        auto pGameBuilder = g_Game->Self()->GetEntity(_builderId);

        if (pGameBuilder != NULL)
            pGameBuilder->CancelConstruction();
    }

    Action::Abort();
}
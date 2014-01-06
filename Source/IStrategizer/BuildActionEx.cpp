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
#include "EntityClassExist.h"

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
Action(ACTIONEX_Build, p_parameters, MaxPrepTime, MaxExecTrialTime, MaxExecTime), _buildStarted(false),  _buildIssued(false)
{
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::OnSucccess(RtsGame* pRtsGame, const WorldClock& p_clock)
{
    if (_buildIssued)
    {
        assert(!_buildArea.IsNull());
        _buildArea.Unlock(this);

        GameEntity *pEntity = pRtsGame->Self()->GetEntity(_builderId);

        if (pEntity)
            pEntity->Unlock(this);
    }
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::OnFailure(RtsGame* pRtsGame, const WorldClock& p_clock)
{
    if (_buildIssued)
    {
        assert(!_buildArea.IsNull());
        _buildArea.Unlock(this);

        GameEntity *pEntity = pRtsGame->Self()->GetEntity(_builderId);

        if (pEntity)
            pEntity->Unlock(this);
    }
}
//////////////////////////////////////////////////////////////////////////
void BuildActionEx::HandleMessage(RtsGame *pRtsGame, Message* p_msg, bool& p_consumed)
{
    if(PlanStepEx::State() == ESTATE_Executing && p_msg->MessageTypeID() == MSG_EntityCreate) 
    {
        EntityCreateMessage*    pMsg = static_cast<EntityCreateMessage*>(p_msg);
        TID                        buildingId;
        GameEntity                *pGameBuilding;
        Vector2                    msgBuildPosition;

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        assert(pMsg && pMsg->Data());
        buildingId = pMsg->Data()->EntityId;

        pGameBuilding = pRtsGame->Self()->GetEntity(buildingId);
        assert(pGameBuilding);

        msgBuildPosition.X = pMsg->Data()->X;
        msgBuildPosition.Y = pMsg->Data()->Y;

        if (msgBuildPosition.X == _buildArea.Pos().X &&
            msgBuildPosition.Y == _buildArea.Pos().Y &&
            pGameBuilding->Type() == _params[PARAM_EntityClassId])
        {
            _buildingId = pGameBuilding->Id();
            _buildStarted = true;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::AliveConditionsSatisfied(RtsGame* pRtsGame)
{
    bool        builderExist = false;
    bool        buildingExist = false;
    bool        isBuilderConstructing = false;
    bool        success = false;
    GameEntity  *pEntity = nullptr;

    assert(PlanStepEx::State() == ESTATE_Executing);

    builderExist = g_Assist.DoesEntityObjectExist(_builderId);

    if (builderExist)
    {
        pEntity = pRtsGame->Self()->GetEntity(_builderId);

        assert(pEntity);
        isBuilderConstructing = (pEntity->Attr(EOATTR_State) == OBJSTATE_Constructing);

        if (!isBuilderConstructing)
        {
            LogInfo("Builder with ID=%d of action %s is not in the constructing state", _builderId, ToString().c_str());
        }
        else
        {
            if (_buildStarted)
            {
                buildingExist = g_Assist.DoesEntityObjectExist(_buildingId);

                if (buildingExist)
                {
                    success = true;
                }
            }
            else
            {
                success = true;
            }
        }
    }
    else
    {
        LogInfo("Builder with ID=%d of action %s does not exist", _builderId, ToString().c_str());
    }

    return success;
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    assert(PlanStepEx::State() == ESTATE_Executing);

    if (_buildStarted)
    {
        int            entityState;
        GameEntity    *pEntity;

        pEntity = pRtsGame->Self()->GetEntity(_buildingId);    
        entityState = pEntity->Attr(EOATTR_State);

        return entityState != OBJSTATE_BeingConstructed;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
bool BuildActionEx::ExecuteAux(RtsGame* pRtsGame, const WorldClock& p_clock)
{
    EntityClassType        buildingType;
    GameEntity            *pGameBuilder;
    AbstractAdapter        *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool                bOk = false;

    // Adapt builder
    _builderId = pAdapter->AdaptWorkerForBuild();

    if (_builderId != INVALID_TID)
    {

        buildingType = (EntityClassType)_params[PARAM_EntityClassId];

        // Initialize build state
        _buildStarted = false;

        // Adapt build position
        assert(pAdapter);
        _buildArea = pAdapter->AdaptPositionForBuilding(buildingType);

        // Issue build order
        pGameBuilder = pRtsGame->Self()->GetEntity(_builderId);
        assert(pGameBuilder);

        bOk = pGameBuilder->Build(buildingType, _buildArea.Pos());

        if (bOk)
        {
            _buildIssued = true;
            pGameBuilder->Lock(this);
            _buildArea.Lock(this);
        }
    }

    return bOk;
}
//----------------------------------------------------------------------------------------------
void BuildActionEx::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], 1, true));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void BuildActionEx::InitializePreConditions()
{
    EntityClassType builderType = g_Game->Self()->GetWorkerType();
    EntityClassType buildingType = (EntityClassType)_params[PARAM_EntityClassId];
    vector<Expression*> m_terms;

    m_terms.push_back(new EntityClassExist(PLAYER_Self, builderType, 1, true));
    g_Assist.GetPrerequisites(buildingType, PLAYER_Self, m_terms);
    _preCondition = new And(m_terms);
}
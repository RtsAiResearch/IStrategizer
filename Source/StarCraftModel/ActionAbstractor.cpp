#include "ActionAbstractor.h"

#include "ActionFactory.h"
#include "Action.h"
#include "StarCraftGame.h"
#include "WorldMap.h"
#include "DefinitionCrossMapping.h"
#include "StarCraftEntity.h"

using namespace BWAPI;
using namespace IStrategizer;
using namespace StarCraftModel;

PlanStepParameters ActionAbstractor::GetAbstractedParameter(RtsGame& p_RtsGame, const Unit trainee, const Unit trainer) const
{
    return GetAbstractedParameterAux(p_RtsGame, ActionFactory::Instance(p_RtsGame).GetAction(ACTIONEX_Train)->Parameters(), trainee, trainer);
}
//////////////////////////////////////////////////////////////////////////
PlanStepParameters ActionAbstractor::GetAbstractedParameter(RtsGame& p_RtsGame, ActionType actionType, const Unit unit) const
{
    return GetAbstractedParameterAux(p_RtsGame, ActionFactory::Instance(p_RtsGame).GetAction(actionType)->Parameters(), unit, nullptr);
}
//////////////////////////////////////////////////////////////////////////
PlanStepParameters ActionAbstractor::GetAbstractedParameterAux(RtsGame& p_RtsGame, PlanStepParameters actionParameters, const Unit unit, const Unit trainer) const
{
    assert(unit);
    IStrategizer::Vector2 position;

    Order order = unit->getOrder();
        
    if (order == Orders::PlaceBuilding || order == Orders::AttackMove ||
        order == Orders::AttackUnit || order == Orders::Move)
    {
        position.X = unit->getOrderTargetPosition().x;
        position.Y = unit->getOrderTargetPosition().y;
    } else if (order == Orders::Upgrade || order == Orders::ResearchTech ||
        trainer != nullptr)
    {
        position.X = unit->getPosition().x;
        position.Y = unit->getPosition().y;
    }
    
    p_RtsGame.Map()->UpdateAux(p_RtsGame);
    p_RtsGame.Map()->GetCellFeatureFromWorldPosition(position)->To(actionParameters);
    
    if (actionParameters.count(PARAM_EntityClassId) > 0)
    {
        if(unit->getOrder() == Orders::PlaceBuilding)
        {
            actionParameters[PARAM_EntityClassId] = g_Database.EntityMapping.GetByFirst(unit->getBuildType().getID());
        }
        else
        {
            actionParameters[PARAM_EntityClassId] = g_Database.EntityMapping.GetByFirst(unit->getType().getID());
        }
    }
    
    if (actionParameters.count(PARAM_TargetEntityClassId) > 0)
    {
        Unit pTarget = unit->getOrderTarget();

        if (pTarget != nullptr)
        {
            TID targetId = unit->getOrderTarget()->getType().getID();
            actionParameters[PARAM_TargetEntityClassId] = g_Database.EntityMapping.GetByFirst(targetId);
        }
    }

    if (actionParameters.count(PARAM_ObjectStateType) > 0)
    {
        StarCraftEntity *pStarCraftEntity = new StarCraftEntity(unit);
        actionParameters[PARAM_ObjectStateType] = pStarCraftEntity->Attr(EOATTR_State);
    }

    if (actionParameters.count(PARAM_ResearchId) > 0)
    {
        if (order == Orders::Upgrade)
        {
            actionParameters[PARAM_ResearchId] = g_Database.UpgradeMapping.GetByFirst(unit->getUpgrade());
        }
        else if (order == Orders::ResearchTech)
        {
            actionParameters[PARAM_ResearchId] = g_Database.UpgradeMapping.GetByFirst(unit->getTech());
        }
    }
    
    return actionParameters;
}
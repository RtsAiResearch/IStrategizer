#include "ActionAbstractor.h"

#include "ActionFactory.h"
#include "Action.h"
#include "RtsGame.h"
#include "WorldMap.h"
#include "DefinitionCrossMapping.h"
#include "StarCraftEntity.h"

using namespace BWAPI;
using namespace IStrategizer;
using namespace StarCraftModel;

PlanStepParameters ActionAbstractor::GetAbstractedParameter(const Unit trainee, const Unit trainer) const
{
    return GetAbstractedParameterAux(g_ActionFactory.GetAction(ACTIONEX_Train)->Parameters(), trainee, trainer);
}
//////////////////////////////////////////////////////////////////////////
PlanStepParameters ActionAbstractor::GetAbstractedParameter(ActionType actionType, const Unit unit) const
{
    return GetAbstractedParameterAux(g_ActionFactory.GetAction(actionType)->Parameters(), unit, nullptr);
}
//////////////////////////////////////////////////////////////////////////
PlanStepParameters ActionAbstractor::GetAbstractedParameterAux(PlanStepParameters actionParameters, const Unit unit, const Unit trainer) const
{
    _ASSERTE(unit);
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
    
    g_Game->Map()->UpdateAux();
    g_Game->Map()->GetCellFeatureFromWorldPosition(position)->To(actionParameters);
    
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
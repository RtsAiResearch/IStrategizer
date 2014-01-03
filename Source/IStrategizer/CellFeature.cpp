#include "CellFeature.h"

#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "RtsGame.h"
#include "MetaData.h"
#include <cmath>

using namespace IStrategizer;

CellFeature::CellFeature(const PlanStepParameters& p_parameters)
{
	m_alliedBuildingDescription.m_numberOfBuildings = p_parameters.at(PARAM_AlliedBuildingsCount);
	m_alliedBuildingDescription.m_numberOfCriticalBuildings = p_parameters.at(PARAM_AlliedCriticalBuildingsCount);
	m_alliedForceDescription.m_numberOfUnits = p_parameters.at(PARAM_AlliedUnitsCount);
	m_alliedForceDescription.m_totalDamage = p_parameters.at(PARAM_AlliedUnitsTotalDamage);
	m_alliedForceDescription.m_totalHP = p_parameters.at(PARAM_AlliedUnitsTotalHP);
	m_enemyBuildingDescription.m_numberOfBuildings = p_parameters.at(PARAM_EnemyBuildingsCount);
	m_enemyBuildingDescription.m_numberOfCriticalBuildings = p_parameters.at(PARAM_EnemyCriticalBuildingsCount);
	m_enemyForceDescription.m_numberOfUnits = p_parameters.at(PARAM_EnemyUnitsCount);
	m_enemyForceDescription.m_totalDamage = p_parameters.at(PARAM_EnemyUnitsTotalDamage);
	m_enemyForceDescription.m_totalHP = p_parameters.at(PARAM_EnemyUnitsTotalHP);
	m_resourceDescription.m_numberOfPrimary = p_parameters.at(PARAM_NumberOfPrimaryResources);
	m_resourceDescription.m_numberOfSecondary = p_parameters.at(PARAM_NumberOfSecondaryResources);
	m_resourceDescription.m_numberOfSupply = p_parameters.at(PARAM_NumberOfSupplyResources);
	m_distanceFromBase = p_parameters.at(PARAM_DistanceToBase);
	m_distanceFromEnemyBase = p_parameters.at(PARAM_DistanceToEnemyBase);
}
//----------------------------------------------------------------------------------------------
void CellFeature::To(PlanStepParameters& p_parameters) const
{
	p_parameters[PARAM_AlliedBuildingsCount] = m_alliedBuildingDescription.m_numberOfBuildings;
	p_parameters[PARAM_AlliedCriticalBuildingsCount] = m_alliedBuildingDescription.m_numberOfCriticalBuildings;
	p_parameters[PARAM_AlliedUnitsCount] = m_alliedForceDescription.m_numberOfUnits;
	p_parameters[PARAM_AlliedUnitsTotalDamage] = m_alliedForceDescription.m_totalDamage;
	p_parameters[PARAM_AlliedUnitsTotalHP] = m_alliedForceDescription.m_totalHP;
	p_parameters[PARAM_EnemyBuildingsCount] = m_enemyBuildingDescription.m_numberOfBuildings;
	p_parameters[PARAM_EnemyCriticalBuildingsCount] = m_enemyBuildingDescription.m_numberOfCriticalBuildings;
	p_parameters[PARAM_EnemyUnitsCount] = m_enemyForceDescription.m_numberOfUnits;
	p_parameters[PARAM_EnemyUnitsTotalDamage] = m_enemyForceDescription.m_totalDamage;
	p_parameters[PARAM_EnemyUnitsTotalHP] = m_enemyForceDescription.m_totalHP;
	p_parameters[PARAM_NumberOfPrimaryResources] = m_resourceDescription.m_numberOfPrimary;
	p_parameters[PARAM_NumberOfSecondaryResources] = m_resourceDescription.m_numberOfSecondary;
	p_parameters[PARAM_NumberOfSupplyResources] = m_resourceDescription.m_numberOfSupply;
	p_parameters[PARAM_DistanceToBase] = m_distanceFromBase;
	p_parameters[PARAM_DistanceToEnemyBase] = m_distanceFromEnemyBase;
}
//----------------------------------------------------------------------------------------------
void CellFeature::AddEntity(GameEntity *p_entity, bool p_isAllied)
{
	if(g_Game->GetEntityType(p_entity->Type())->Attr(ECATTR_IsBuilding))
	{
		if(p_isAllied)
			m_alliedBuildingDescription.AddEntity(p_entity);
		else
			m_enemyBuildingDescription.AddEntity(p_entity);
			
	}
	else if(g_Game->GetEntityType(p_entity->Type())->Attr(ECATTR_CanAttack))
	{
		if(p_isAllied)
			m_alliedForceDescription.AddEntity(p_entity);
		else
			m_enemyForceDescription.AddEntity(p_entity);
	}
	else
	{
		m_resourceDescription.AddEntity(p_entity);
	}
}
//----------------------------------------------------------------------------------------------
void CellFeature::RemoveEntity(GameEntity *p_entity, bool p_isAllied)
{
	if(g_Game->GetEntityType(p_entity->Type())->Attr(ECATTR_IsBuilding))
	{
		if(p_isAllied)
			m_alliedBuildingDescription.RemoveEntity(p_entity);
		else
			m_enemyBuildingDescription.RemoveEntity(p_entity);

	}
	else if(g_Game->GetEntityType(p_entity->Type())->Attr(ECATTR_CanAttack))
	{
		if(p_isAllied)
			m_alliedForceDescription.RemoveEntity(p_entity);
		else
			m_enemyForceDescription.RemoveEntity(p_entity);
	}
	else
	{
		m_resourceDescription.RemoveEntity(p_entity);
	}
}
//----------------------------------------------------------------------------------------------
void CellFeature::Clear()
{
	m_alliedBuildingDescription.Clear();
	m_enemyBuildingDescription.Clear();
	m_alliedForceDescription.Clear();
	m_enemyForceDescription.Clear();
	m_resourceDescription.Clear();
	m_distanceFromBase = 0;
	m_distanceFromEnemyBase = 0;
}
//----------------------------------------------------------------------------------------------
float CellFeature::GetDistance(CellFeature *p_other)
{
	float res = 0.0;
	float alliedBuildingDistance = m_alliedBuildingDescription.GetDistance(&(p_other->m_alliedBuildingDescription));
	float enemyBuildingDistance = m_enemyBuildingDescription.GetDistance(&(p_other->m_enemyBuildingDescription));
	float alliedForceDistance = m_alliedForceDescription.GetDistance(&(p_other->m_alliedForceDescription));
	float enemyForceDistance = m_enemyForceDescription.GetDistance(&(p_other->m_enemyForceDescription));
	float resourceDistance = m_resourceDescription.GetDistance(&(p_other->m_resourceDescription));
	float distanceFromBase = GetBaseDistance(this->m_distanceFromBase, p_other->m_distanceFromBase);
	float distanceFromEnemyBase = GetBaseDistance(this->m_distanceFromEnemyBase, p_other->m_distanceFromEnemyBase);
	
	res += alliedBuildingDistance;
	res += enemyBuildingDistance;
	res += alliedForceDistance;
	res += enemyForceDistance;
	res += resourceDistance;
	res += distanceFromBase;
	res += distanceFromEnemyBase;

	return sqrt(res);
}
//----------------------------------------------------------------------------------------------
void CellFeature::CalculateDistanceToBases(Vector2 cellWorldPosition)
{
	vector<TID> bases;

	g_Game->Enemy()->GetBases(bases);
	CalculateDistanceToBasesAux(cellWorldPosition, bases, m_distanceFromEnemyBase);
	g_Game->Self()->GetBases(bases);
	CalculateDistanceToBasesAux(cellWorldPosition, bases, m_distanceFromBase);
}
//----------------------------------------------------------------------------------------------
void CellFeature::CalculateDistanceToBasesAux(Vector2 cellWorldPosition, vector<TID> bases, double& distance)
{
	assert(bases.size() > 0);
	TID baseId = bases[0];
	GameEntity* pBase = g_Game->Self()->GetEntity(baseId);

	if (pBase == nullptr)
		pBase = g_Game->Enemy()->GetEntity(baseId);

	assert(pBase);

	distance = cellWorldPosition.Distance(pBase->GetPosition());
}
//----------------------------------------------------------------------------------------------
float CellFeature::GetBaseDistance(double firstBase, double secondBase) const
{
	return pow((float)(firstBase - secondBase), 2);
}
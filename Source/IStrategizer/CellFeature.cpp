#include "CellFeature.h"
#include "GameEntity.h"
#include <cmath>
#include "GameType.h"
#include "RtsGame.h"
#include "MetaData.h"

using namespace IStrategizer;

void CellFeature::InitializeAddressesAux()
{

	AddMemberAddress(4,
		&m_alliedBuildingDescription,
		&m_alliedForceDescription,
		&m_enemyBuildingDescription,
		&m_enemyForceDescription);
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
}
//----------------------------------------------------------------------------------------------
double CellFeature::GetDistance(CellFeature *p_other)
{
	double res = 0.0;
	double alliedBuildingDistance	= m_alliedBuildingDescription.GetDistance(&(p_other->m_alliedBuildingDescription));
	double enemyBuildingDistance	= m_enemyBuildingDescription.GetDistance(&(p_other->m_enemyBuildingDescription));
	double alliedForceDistance		= m_alliedForceDescription.GetDistance(&(p_other->m_alliedForceDescription));
	double enemyForceDistance		= m_enemyForceDescription.GetDistance(&(p_other->m_enemyForceDescription));
	double resourceDistance			= m_resourceDescription.GetDistance(&(p_other->m_resourceDescription));
	
	res += alliedBuildingDistance*alliedBuildingDistance;
	res += enemyBuildingDistance*enemyBuildingDistance;
	res += alliedForceDistance*alliedForceDistance;
	res += enemyForceDistance*enemyForceDistance;
	res += resourceDistance*resourceDistance;

	return sqrt(res);
}

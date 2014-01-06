#include "CollectResourceGoal.h"
#include "And.h"
#include "False.h"

using namespace IStrategizer;

CollectResourceGoal::CollectResourceGoal() : GoalEx(GOALEX_CollectResource)
{
    _params[PARAM_ResourceId] = RESOURCE_START;
    _params[PARAM_Amount] = 0;
}
//----------------------------------------------------------------------------------------------
CollectResourceGoal::CollectResourceGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_CollectResource, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void CollectResourceGoal::InitializePostConditions()
{
    //map<EntityClassType, int> m_baseTypeRequiredBuildings;
    //EntityClassType m_baseBuildingClassId;

    //g_Assist.GetRequiredBuildingsForBaseType(PLAYER_Self, (BaseType)_params[PARAM_BaseTypeId], m_baseTypeRequiredBuildings);
    //g_Assist.GetTireBaseBuildingId(PLAYER_Self, (BaseType)_params[PARAM_BaseTypeId], m_baseBuildingClassId);

    //m_baseTypeRequiredBuildings[m_baseBuildingClassId] = 1;

    vector<Expression*> m_termsPost;
    m_termsPost.push_back(new False);
    //m_termsPost.resize(m_baseTypeRequiredBuildings.size());

    //int m_index = 0;
    //for (map<EntityClassType, int>::const_iterator m_itr = m_baseTypeRequiredBuildings.begin(); m_itr != m_baseTypeRequiredBuildings.end(); m_itr++)
    // m_termsPost[m_index++] = new EntityClassExist(PLAYER_Self, (*m_itr).first, (*m_itr).second, true);

    _postCondition = new And(m_termsPost);
}
//----------------------------------------------------------------------------------------------
bool CollectResourceGoal::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    return false;
}
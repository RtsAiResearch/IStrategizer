#ifndef GOALSATISFACTIONROW_H
	#include "GoalSatisfactionRow.h"
#endif

#ifndef METADATA_H
	#include "MetaData.h"
#endif

#ifndef ENGINEDATA_H
	#include "EngineData.h"
#endif

#ifndef GOALFACTORY_H
	#include "GoalFactory.h"
#endif

#include <cmath>

//----------------------------------------------------------------------------------------------
int GoalSatisfactionRow::GetRowSize()
{
    return _rowSize;
}
//----------------------------------------------------------------------------------------------
void GoalSatisfactionRow::Initialize(PlayerType p_humanPlayer, PlayerType p_staticAIBot)
{
    Level m_level;
    int m_goalIdx;
    Group m_group;

    _rowSize = 0;;

    _goalParamGraph.resize(COUNT(GoalType));

    // Goal: WinWargus 
    m_goalIdx = INDEX(GOALEX_WinGame, GoalType);
    m_level.clear();
    for(int paramIdx = START(StrategyType); paramIdx < END(StrategyType); ++paramIdx)
    {
        m_group.clear();
        m_group.push_back(make_pair(PARAM_StrategyTypeId, paramIdx));
        m_level.push_back(m_group);
    }

    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);

    m_level.clear();
    m_group.clear();
    m_group.push_back(make_pair(PARAM_PlayerId, p_humanPlayer));
    m_level.push_back(m_group);

    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);

    _goalParamGraph[m_goalIdx].EvaluatePath = new Predicate<GoalSatisfactionRow>(this, &GoalSatisfactionRow::EvaluatePath);
    _rowSize += COUNT(StrategyType);
    _parameterStart[PARAM_StrategyTypeId] = STRTYPE_START;

    // Goal: TrainForce
    m_goalIdx = INDEX(GOALEX_TrainForce, GoalType);
    m_level.clear();
    m_group.clear();
    for(int paramIdx = START(ForceSizeType); paramIdx < END(ForceSizeType); ++paramIdx)
    {
        m_group.push_back(make_pair(PARAM_ForceSizeId, paramIdx));
    }
    m_level.push_back(m_group);
    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);
    _goalParamGraph[m_goalIdx].EvaluatePath = new Predicate<GoalSatisfactionRow>(this, &GoalSatisfactionRow::EvaluatePath);
    _rowSize += COUNT(ForceSizeType);
    _parameterStart[PARAM_ForceSizeId] = FORCESIZE_START;

    m_level.clear();
    m_group.clear();
    m_group.push_back(make_pair(PARAM_PlayerId, p_humanPlayer));
    m_level.push_back(m_group);
    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);
    _parameterStart[PARAM_PlayerId] = PLAYER_START;

    // Goal: BuildBase
    m_goalIdx = INDEX(GOALEX_BuildBase, GoalType);
    m_level.clear();
    for(int paramIdx = START(BaseType); paramIdx < END(BaseType); ++paramIdx)
    {
		m_group.clear();
        m_group.push_back(make_pair(PARAM_BaseTypeId, paramIdx));
		m_level.push_back(m_group);
    }
    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);
    _goalParamGraph[m_goalIdx].EvaluatePath = new Predicate<GoalSatisfactionRow>(this, &GoalSatisfactionRow::EvaluatePath);
    _rowSize += COUNT(BaseType);
    _parameterStart[PARAM_BaseTypeId] = BASETYPE_START;

	m_level.clear();
	m_group.clear();
	m_group.push_back(make_pair(PARAM_PlayerId, p_humanPlayer));
	m_level.push_back(m_group);
	_goalParamGraph[m_goalIdx].Levels().push_back(m_level);
	_parameterStart[PARAM_PlayerId] = PLAYER_START;

    // Goal: AttackEnemy
    m_goalIdx = INDEX(GOALEX_AttackEnemy, GoalType);
    m_level.clear();
    for(int paramIdx = START(AttackType); paramIdx < END(AttackType); ++paramIdx)
    {
        m_group.clear();
        m_group.push_back(make_pair(PARAM_AttackTypeId, paramIdx));
        m_level.push_back(m_group);
    }
    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);
    _parameterStart[PARAM_AttackTypeId] = ATTACK_START;

    m_level.clear();
    m_group.clear();
    for(int paramIdx = START(ForceSizeType); paramIdx < END(ForceSizeType); ++paramIdx)
    {
        m_group.push_back(make_pair(PARAM_ForceSizeId, paramIdx));
    }
    m_level.push_back(m_group);
    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);
    _rowSize += COUNT(ForceSizeType) * COUNT(AttackType);

    m_level.clear();
    m_group.clear();
    m_group.push_back(make_pair(PARAM_PlayerId, p_humanPlayer));
    m_level.push_back(m_group);
    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);
    _parameterStart[PARAM_PlayerId] = PLAYER_START;

    m_level.clear();
    m_group.clear();
    m_group.push_back(make_pair(PARAM_TargetPlayerId, p_staticAIBot));
    m_level.push_back(m_group);
    _goalParamGraph[m_goalIdx].Levels().push_back(m_level);
    _parameterStart[PARAM_TargetPlayerId] = PLAYER_START;
    
    _goalParamGraph[m_goalIdx].EvaluatePath = new Predicate<GoalSatisfactionRow>(this, &GoalSatisfactionRow::EvaluatePath);

    GeneratePossibleParams();
}
//----------------------------------------------------------------------------------------------
void GoalSatisfactionRow::GeneratePossibleParams()
{
    // Perform a DFS traversal
    int m_lastParamGoalIdx = -1;
    for(int i = 0; i < _goalParamGraph.size(); ++i)
    {
        GeneratePossibleParamsAux(i, 0, i, m_lastParamGoalIdx);
    }
}
//----------------------------------------------------------------------------------------------
void GoalSatisfactionRow::GeneratePossibleParamsAux(int p_abstractGoalIdx, int p_level, TPathKey p_path, int& p_lastParamGoalIdx)
{
    // Base Case: reached a leaf, and we have a complete path
    if(p_level == _goalParamGraph[p_abstractGoalIdx].Levels().size())
    {
        ++p_lastParamGoalIdx;
        _keyToIdxMapping[p_path] = p_lastParamGoalIdx;
    }
    else
    {
        Level& m_currentLevel = _goalParamGraph[p_abstractGoalIdx].Levels()[p_level];

        int m_childIdx = -1;
        TPathKey m_newPath;
        int m_numBits;

        for(int groupIdx = 0; groupIdx < m_currentLevel.size(); ++groupIdx)
        {
            Group& m_currentGroup = m_currentLevel[groupIdx];
            for(int elementIdx = 0; elementIdx < m_currentGroup.size(); ++elementIdx)
            {
                m_childIdx = m_currentGroup[elementIdx].second - _parameterStart[(ParameterType)m_currentGroup[elementIdx].first];

                if(m_childIdx == 0)
                {
                    m_numBits = 1;
                }
                else
                {
                    m_numBits = (int)(log10((float)m_childIdx) / log10(2.0));
                    ++m_numBits;
                }

                m_newPath = p_path << m_numBits;
                m_newPath |= m_childIdx;

                GeneratePossibleParamsAux(p_abstractGoalIdx, p_level + 1, m_newPath, p_lastParamGoalIdx);
            }
        }
    }
}
//----------------------------------------------------------------------------------------------
void GoalSatisfactionRow::Compute(unsigned long p_gameCycle, vector<int>& p_row)
{
    _row            = &p_row;
    _currentGoal    = -1;
    _lastGameCycle  = p_gameCycle;

    for(vector<CombinatoricsAndOrGraph<GoalSatisfactionRow>>::iterator goalItr = _goalParamGraph.begin();
        goalItr != _goalParamGraph.end();
        goalItr++)
    {
        ++_currentGoal;
        goalItr->Evaluate();
    }
}
//----------------------------------------------------------------------------------------------
bool GoalSatisfactionRow::EvaluatePath()
{
    list<NodeType>& m_path = _goalParamGraph[_currentGoal].Path();
    PlanStepParameters m_params;
    
    for(list<NodeType>::iterator itr = m_path.begin();
        itr != m_path.end();
        itr++)
    {
        m_params[(ParameterType)itr->first] = itr->second;
    }
    
    int m_pathId = Hash(_currentGoal, m_path);
    GoalEx* m_goal = GetGoal(m_pathId, m_path);
    m_goal->Update(_lastGameCycle);
    
    int m_goalIdx = _keyToIdxMapping[m_pathId];
    bool m_res = _goals[m_pathId]->State() == ESTATE_Succeeded;
    _row->operator [](m_goalIdx) = m_res;

    return m_res;
}
//----------------------------------------------------------------------------------------------
TPathKey GoalSatisfactionRow::Hash(int p_goalIdx, list<NodeType>& p_parameters)
{
    assert(!p_parameters.empty());

    TPathKey m_key = p_goalIdx;
    int m_numBits;
    int idx;
    for(list<NodeType>::iterator itr = p_parameters.begin();
        itr != p_parameters.end();
        itr++)
    {
        idx = (float)itr->second - _parameterStart[(ParameterType)itr->first];

        if(idx == 0)
        {
            m_numBits = 1;
        }
        else
        {
            m_numBits = log10((float)idx) / log10(2.0);
            ++m_numBits;
        }
      
        m_key <<= m_numBits;
        m_key |= idx;
    }

    return m_key;
}
//----------------------------------------------------------------------------------------------
GoalEx* GoalSatisfactionRow::GetGoal(TPathKey p_pathId, list<NodeType>& p_path)
{
    if(_goals.find(p_pathId) == _goals.end())
    {
        PlanStepParameters m_params;

        for(list<NodeType>::iterator itr = p_path.begin();
            itr != p_path.end();
            itr++)
        {
            m_params[(ParameterType)itr->first] = itr->second;
        }

        GoalEx* m_goal = g_GoalFactory.GetGoal((GoalType)GET(_currentGoal, GoalType), m_params);
        assert(m_goal != NULL);
        _goals[p_pathId] = m_goal;
    }


    return _goals[p_pathId];
}
//----------------------------------------------------------------------------------------------
GoalEx* GoalSatisfactionRow::GetGoal(int p_goalIdx)
{
    GoalEx* m_goal = NULL;
    for(map<TPathKey, int>::iterator itr = _keyToIdxMapping.begin();
        itr != _keyToIdxMapping.end();
        itr++)
    {
        if(itr->second == p_goalIdx)
        {
            m_goal = _goals[itr->first];
            break;
        }
    }

    // this function should return a goal of and only if it succeeded during a previous goal satisfaction row computation
    assert(m_goal != NULL);
    return m_goal;
}
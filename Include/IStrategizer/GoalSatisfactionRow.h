#ifndef GOALSATISFACTIONROW_H
#define GOALSATISFACTIONROW_H

#ifndef COMBINATORICSANDORGRAPH_H
	#include "CombinatoricsAndOrGraph.h"
#endif

#ifndef METADATA_H
    #include "MetaData.h"
#endif

#ifndef GOALEX_H
	#include "GoalEx.h"
#endif

#include <map>
using namespace std;

typedef unsigned long long TPathKey;

class GoalSatisfactionRow
{
    vector<CombinatoricsAndOrGraph<GoalSatisfactionRow>> _goalParamGraph;
    map<ParameterType, int> _parameterStart;
    unsigned long           _lastGameCycle;
    vector<int>*            _row;
    int                     _rowSize;
    int                     _currentGoal;
    map<TPathKey, GoalEx*>  _goals;
    map<TPathKey, int>      _keyToIdxMapping;

protected:
    bool        EvaluatePath();
    TPathKey    Hash(int p_goalIdx, list<NodeType>& p_parameters);
    void        GeneratePossibleParams();
    void        GeneratePossibleParamsAux(int p_abstractGoalIdx, int p_level, TPathKey p_path, int& p_lastParamGoalIdx);
    GoalEx*     GetGoal(TPathKey p_pathId, list<NodeType>& p_path);
public:
    void    Initialize(PlayerType p_humanPlayer, PlayerType p_staticAIBot);
    void    Compute(unsigned long p_gameCycle, vector<int>& p_row);
    int     GetRowSize();
    GoalEx* GetGoal(int p_goalIdx);

};

#endif // GOALSATISFACTIONROW_H
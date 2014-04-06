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

#ifndef GAMETRACE_H
#include "GameTrace.h"
#endif

#include <map>

namespace IStrategizer
{
    typedef unsigned long long TPathKey;

  // N X 1 matrix of booleans, where N = number supported goals, 
  // where GoalMatrix[i] indicates whether the goal was satisfied or not
  typedef std::vector<bool> GoalMatrixRow;

    class GoalMatrixRowEvaluator
    {
        vector<CombinatoricsAndOrGraph<GoalMatrixRowEvaluator>> _goalParamGraph;
        std::map<ParameterType, int> _parameterStart;
        unsigned long _lastGameCycle;
        GoalMatrixRow *_row;
        int _rowSize;
        int _currentGoal;
        std::map<TPathKey, GoalEx*> _goals;
        std::map<TPathKey, int> _keyToIdxMapping;

    protected:
        bool EvaluatePath();
        TPathKey Hash(int p_goalIdx, std::list<NodeType>& p_parameters);
        void GeneratePossibleParams();
        void GeneratePossibleParamsAux(int p_abstractGoalIdx, unsigned p_level, TPathKey p_path, int& p_lastParamGoalIdx);
        GoalEx* GetGoal(TPathKey p_pathId, std::list<NodeType>& p_path);
    public:
        void Initialize(PlayerType p_humanPlayer, PlayerType p_staticAIBot);
        void Compute(unsigned p_gameCycle, GoalMatrixRow& p_row);
        size_t GetRowSize() const { return (size_t)_rowSize; }
        GoalEx* GetGoal(int p_goalIdx);
    };
}

#endif // GOALSATISFACTIONROW_H

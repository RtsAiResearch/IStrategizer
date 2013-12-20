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

namespace IStrategizer
{
	typedef unsigned long long TPathKey;

	class GoalSatisfactionRow
	{
		vector<CombinatoricsAndOrGraph<GoalSatisfactionRow>> _goalParamGraph;
		std::map<ParameterType, int> _parameterStart;
		unsigned long _lastGameCycle;
		std::vector<int> *_row;
		int _rowSize;
		int _currentGoal;
		std::map<TPathKey, GoalEx*> _goals;
		std::map<TPathKey, int> _keyToIdxMapping;

	protected:
		bool        EvaluatePath();
		TPathKey    Hash(int p_goalIdx, std::list<NodeType>& p_parameters);
		void        GeneratePossibleParams();
		void        GeneratePossibleParamsAux(int p_abstractGoalIdx, unsigned p_level, TPathKey p_path, int& p_lastParamGoalIdx);
		GoalEx*     GetGoal(TPathKey p_pathId, std::list<NodeType>& p_path);
	public:
		void    Initialize(PlayerType p_humanPlayer, PlayerType p_staticAIBot);
		void    Compute(unsigned long p_gameCycle, std::vector<int>& p_row);
		int     GetRowSize();
		GoalEx* GetGoal(int p_goalIdx);
	};
}

#endif // GOALSATISFACTIONROW_H
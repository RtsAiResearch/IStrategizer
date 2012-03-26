#ifndef RETRIEVEREX_H
#define RETRIEVEREX_H

#ifndef ABSTRACTRETRIEVER_H
#include "AbstractRetriever.h"
#endif

#include <hash_map>
using namespace stdext;

class CaseEx;
class GoalEx;
class GameStateEx;
class CaseBaseEx;

namespace MetaData
{
	enum GoalType;
}

namespace OLCBP
{
	using namespace MetaData;

	class AbstractRetainer;

    class RetrieverEx : public AbstractRetriever
    {
	public:
		RetrieverEx(AbstractRetainer *p_pRetainer);
		CaseEx* Retrieve(const GoalEx* p_goal, const GameStateEx* p_gameState);

    protected:
        float   GoalSimilarity(const GoalEx* p_g1, const GoalEx* p_g2);
        float   StateSimilarity(const GameStateEx* p_gs1, const GameStateEx* p_gs2);
        float   CaseRelevance(const CaseEx* p_case, const GoalEx* p_goal, const GameStateEx* p_gameState);
        void    ExecuteCommand(const char* p_cmd);
		void	BuildCaseCluster();

		hash_map<GoalType, vector<CaseEx*>> _caseCluster;
    };
}


#endif // RETRIEVEREX_H
#ifndef RETRIEVEREX_H
#define RETRIEVEREX_H

#ifndef ABSTRACTRETRIEVER_H
#include "AbstractRetriever.h"
#endif

#include <hash_map>

namespace IStrategizer
{
    enum GoalType;
    class AbstractRetainer;
    class CaseEx;
    class GoalEx;
    class CaseBaseEx;
    class RtsGame;

    class RetrieverEx : public AbstractRetriever
    {
    public:
        RetrieverEx(AbstractRetainer *p_pRetainer);
        CaseEx* Retrieve(const GoalEx* pGoal, const RtsGame* pGameState, const std::set<CaseEx*>& exclusion);

    protected:
        float   GoalSimilarity(const GoalEx* p_g1, const GoalEx* p_g2);
        float   StateSimilarity(const RtsGame* p_gs1, const RtsGame* p_gs2);
        float   CaseRelevance(const CaseEx* p_case, const GoalEx* p_goal, const RtsGame* p_gameState);
        void    ExecuteCommand(const char* p_cmd);
        void BuildCaseCluster();

        stdext::hash_map<GoalType, std::vector<CaseEx*>> _caseCluster;
    };
}

#endif // RETRIEVEREX_H

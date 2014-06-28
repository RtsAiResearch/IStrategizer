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
        CaseEx* Retrieve(AbstractRetriever::RetrieveOptions options);

    protected:
        float GoalDistance(const GoalEx* pCaseGoal, AbstractRetriever::RetrieveOptions options);
        float StateSimilarity(RtsGame* pCaseGameState, AbstractRetriever::RetrieveOptions options);
        float CaseRelevance(const CaseEx* pCase, AbstractRetriever::RetrieveOptions options);
        void ExecuteCommand(const char* p_cmd);
        void BuildCaseCluster();

        stdext::hash_map<GoalType, std::vector<CaseEx*>> _caseCluster;

    private:
        std::map<GoalType, int> m_lastGoalTypeInx;
    };
}

#endif // RETRIEVEREX_H

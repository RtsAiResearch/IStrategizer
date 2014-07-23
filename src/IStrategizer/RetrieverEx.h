#ifndef RETRIEVEREX_H
#define RETRIEVEREX_H

#include "AbstractRetriever.h"
#include <map>
#include <unordered_map>
#include <memory>

namespace IStrategizer
{
    enum GoalType;
    class AbstractRetainer;
    class CaseEx;
    class GoalEx;
    class CaseBaseEx;
    class RtsGame;

    class CasebaseNumericalLimits
    {
    public:
        struct MinMaxVal
        {
            int Min;
            int Max;
            int Diff;
        };

        void ComputeLimits(_In_ const CaseBaseEx& cb);
        float Normalize(_In_ const MinMaxVal& limit, _In_ int val);
        const MinMaxVal& GetLimit(_In_ ParameterType type) const { return m_goalParamsMinMax.at(type); }

    private:
        std::unordered_map<RtsGameModelAttribute, MinMaxVal> m_gameModelAttrMinMax;
        std::unordered_map<ParameterType, MinMaxVal> m_goalParamsMinMax;
    };

    class RetrieverEx : public AbstractRetriever
    {
    public:
        RetrieverEx(AbstractRetainer *p_pRetainer);
        CaseEx* Retrieve(const AbstractRetriever::RetrieveOptions& options);
        bool Init();

    protected:
        float GoalDistance(const GoalEx* pCaseGoal, const AbstractRetriever::RetrieveOptions& options);
        float StateDistance(RtsGame* pCaseGameState, const AbstractRetriever::RetrieveOptions& options);
        float CaseDistance(const CaseEx* pCase, const AbstractRetriever::RetrieveOptions& options);

        std::shared_ptr<GoalEx> GetGoal(_In_ GoalType type, _In_ const PlanStepParameters& params);

        CasebaseNumericalLimits m_gameStateParamsLimits;
        std::map<GoalType, int> m_lastGoalTypeInx;
        CasebaseNumericalLimits m_limits;
    };
}

#endif // RETRIEVEREX_H

///> [Serializable]
#ifndef CONDITIONEX_H
#define CONDITIONEX_H

#include <map>
#include "MetaData.h"
#include "EngineAssist.h"
#include "Comparer.h"
#include "Expression.h"

namespace IStrategizer
{
    ///> class=ConditionEx
    ///> parent=Expression
    class ConditionEx : public Expression
    {
		OBJECT_SERIALIZABLE_P(ConditionEx, Expression, &_conditionType, &m_params, &_playerType);
    protected:
        ///> type=int
        ConditionType _conditionType;
        ///> type=PlanStepParameters
        PlanStepParameters m_params;
        ///> type=int
        PlayerType _playerType;

        inline ConditionEx() {}

    public:
        inline ConditionEx(PlayerType p_player, ConditionType p_conditionType) : Expression(EXPRESSION_Leaf), _conditionType(p_conditionType), _playerType(p_player) { m_params[PARAM_PlayerId] = p_player; }
        inline void Parameter(int p_paramId, int p_newValue) { m_params[(ParameterType)p_paramId] = p_newValue; }
        inline int Type() const { return _conditionType; } 
        inline int Parameter(int p_paramId) { return m_params[(ParameterType)p_paramId]; }
        inline bool ContainsParameter(ParameterType p_parameter) { return (m_params.find(p_parameter) != m_params.end()); }
        inline PlanStepParameters   Parameters() const { return m_params; }
        void Copy(IClonable* p_dest);
        bool PartiallyEqualsAux(const Expression* p_rhs,  MatchSide p_anchor, std::vector<std::pair<Expression*,Expression*>>& p_matchedLeafs) const;
        bool Evaluate(RtsGame& game) { _isEvaluated = _isSatisfied = true; return _isEvaluated && _isSatisfied; }
        bool Equals(const Expression* p_rhs) const;
        virtual bool Consume(int p_amount) = 0;
    };
}

#endif  // CONDITIONEX_H

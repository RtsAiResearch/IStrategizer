///> [Serializable]
#ifndef CONDITIONEX_H
#define CONDITIONEX_H

#include <map>
#include "MetaData.h"
#include "ErrorCode.h"
#include "EngineAssist.h"
#include "Comparer.h"
#include "Expression.h"

namespace IStrategizer
{
    ///> class=ConditionEx
    ///> parent=Expression
    class ConditionEx : public Expression
    {
        OBJECT_MEMBERS_P(Expression, 3, &_conditionType, &_conditionParameters, &_playerType);

    protected:
        ///> type=int
        ConditionType _conditionType;
        ///> type=PlanStepParameters
        PlanStepParameters _conditionParameters;
        ///> type=int
        PlayerType _playerType;

        inline ConditionEx() {}

    public:
        inline ConditionEx(PlayerType p_player, ConditionType p_conditionType) : Expression(EXPRESSION_Leaf), _conditionType(p_conditionType), _playerType(p_player) { _conditionParameters[PARAM_PlayerId] = p_player; }
        inline void Parameter(int p_paramId, int p_newValue) { _conditionParameters[(ParameterType)p_paramId] = p_newValue; }
        inline int Type() const { return _conditionType; } 
        inline int Parameter(int p_paramId) { return _conditionParameters[(ParameterType)p_paramId]; }
        inline bool ContainsParameter(ParameterType p_parameter) { return (_conditionParameters.find(p_parameter) != _conditionParameters.end()); }
        inline PlanStepParameters   Parameters() const { return _conditionParameters; }
        void Copy(IClonable* p_dest);
        bool PartiallyEqualsAux(const Expression* p_rhs,  MatchSide p_anchor, std::vector<std::pair<Expression*,Expression*>>& p_matchedLeafs) const;
        bool Evaluate(RtsGame& game) { _isEvaluated = _isSatisfied = true; return _isEvaluated && _isSatisfied; }
        bool Equals(const Expression* p_rhs) const;
        virtual bool Consume(int p_amount) = 0;
    };
}

#endif  // CONDITIONEX_H

///> [Serializable]
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include "MetaData.h"
#include "IClonable.h"
#include "RtsGame.h"

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

namespace IStrategizer
{
    enum MatchSide
    {
        MATCHSIDE_Left,
        MATCHSIDE_Right
    };

    ///> class=Expression
    class Expression : public Serialization::UserObject, public IClonable
    {
    protected:
        ///> type=int
        ExpressionType _expressionType;
        ///> type=bool
        bool _isEvaluated;
        ///> type=bool
        bool _isSatisfied;

        void InitializeAddressesAux();

    public:
        Expression() {}
        Expression(ExpressionType p_expressionType) : _expressionType(p_expressionType), _isEvaluated(false), _isSatisfied(false) {}
        Expression(Expression& p_expression) : _expressionType(p_expression._expressionType), _isEvaluated(p_expression._isEvaluated), _isSatisfied(p_expression._isSatisfied) {}
        virtual void ResetSatisfaction() { _isSatisfied = false; }
        virtual void Copy(IClonable* p_dest);
        virtual bool Evaluate(RtsGame* pRtsGame) = 0;
        virtual bool Equals(const Expression* p_rhs) const = 0;
        virtual bool PartiallyEquals(const Expression* p_rhs, std::vector<pair<Expression*, Expression*>>& p_matchedLeafs) { return PartiallyEqualsAux(p_rhs, MATCHSIDE_Left, p_matchedLeafs); }
        virtual bool PartiallyEqualsAux(const Expression* p_rhs, MatchSide p_anchor, std::vector<pair<Expression*, Expression*>>& p_matchedLeafs) const = 0;
        virtual IClonable* Clone();
        inline ExpressionType ExpressionType() const { return _expressionType; }
        inline void IsEvaluated(bool val) { _isEvaluated = val; }
        inline void IsSatisfied(bool val) { _isSatisfied = val; }
        inline bool IsEvaluated() const { return _isEvaluated; }
        inline bool IsSatisfied() const { return _isSatisfied; }
        inline bool operator==(Expression* exp) const { Equals(exp); }
    };
}

#endif  // EXPRESSION_H

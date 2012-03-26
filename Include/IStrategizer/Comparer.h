#ifndef COMPARER_H
#define COMPARER_H

#include "MetaData.h"
using namespace MetaData;

class Comparer
{
private:
	inline static bool IsLessThan(int p_lhs, int p_rhs) { return p_lhs < p_rhs; }

	inline static bool IsLessThanOrEqual(int p_lhs, int p_rhs) { return p_lhs <= p_rhs; }

	inline static bool IsGreaterThan(int p_lhs, int p_rhs) { return p_lhs > p_rhs; }

	inline static bool IsGreaterThanOrEqual(int p_lhs, int p_rhs) { return p_lhs >= p_rhs; }

	inline static bool IsEqual(int p_lhs, int p_rhs) { return p_lhs == p_rhs; }

	inline static bool IsNotEqual(int p_lhs, int p_rhs) { return p_lhs != p_rhs; }

public:
     inline static bool Compare(int p_lhs, RelationalOperatorType p_relop, int p_rhs)
	 {
		 switch (p_relop)
		 {
		 case RELOP_LessThan:
			 return IsLessThan(p_lhs, p_rhs);

		 case RELOP_LessThanOrEqual:
			 return IsLessThanOrEqual(p_lhs, p_rhs);

		 case RELOP_GreaterThan:
			 return IsGreaterThan(p_lhs, p_rhs);

		 case RELOP_GreaterThanOrEqual:
			 return IsGreaterThanOrEqual(p_lhs, p_rhs);

		 case RELOP_Equal:
			 return IsEqual(p_lhs, p_rhs);

		 case RELOP_NotEqual:
			 return IsNotEqual(p_lhs, p_rhs);
		 }

		 throw std::exception("Operator not supported");
	 }
};
#endif // COMPARER_H
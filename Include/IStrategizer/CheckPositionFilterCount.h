///> [Serializable]
#ifndef CHECKPOSITIONFILTERCOUNT_H
#define CHECKPOSITIONFILTERCOUNT_H

#include "ConditionEx.h"
#include "MetaData.h"
#include "Vector2.h"

namespace IStrategizer
{
	///> class=CheckPositionFilterCount
	///> parent=ConditionEx
	class CheckPositionFilterCount : public ConditionEx
	{
		OBJECT_SERIALIZABLE(CheckPositionFilterCount);

	private:
		int		_currentCount;
		Vector2	_cellIndex;
	public:
		CheckPositionFilterCount() {}
		CheckPositionFilterCount(PlayerType p_player, int p_filterTypeId, int p_operatorId, int p_value, const Vector2& p_cellIndex);
		int		    CurrentCount() const { return _currentCount; }
		bool	    Evaluate();
		void        Copy(IClonable* p_dest);
		bool		Consume(int p_amount) { return true; }
	};
}

#endif	// CHECKPOSITIONFILTERCOUNT_H
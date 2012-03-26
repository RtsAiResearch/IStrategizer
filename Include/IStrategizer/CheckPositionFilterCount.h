///> [Serializable]
#ifndef CHECKPOSITIONFILTERCOUNT_H
#define CHECKPOSITIONFILTERCOUNT_H

#include "ConditionEx.h"
#include "MetaData.h"
#include "Vector2.h"

using namespace MetaData;


///> class=CheckPositionFilterCount
///> parent=ConditionEx
class CheckPositionFilterCount : public ConditionEx
{
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
    //----------------------------------------------------------------------------------------------
    // Serialization
public:
    string      TypeName()  { return "CheckPositionFilterCount"; }
    int         TypeSize()  { return sizeof(CheckPositionFilterCount); }
    UserObject* Prototype() { return new CheckPositionFilterCount; }	
    //----------------------------------------------------------------------------------------------
};
#endif	// CHECKPOSITIONFILTERCOUNT_H
///> [Serializable]
#ifndef CHECKCOLONYFILTERCOUNT_H
#define CHECKCOLONYFILTERCOUNT_H

#include "ConditionEx.h"
#include "MetaData.h"
#include "Colony.h"
using namespace MetaData;

///> class=CheckColonyFilterCount
///> parent=ConditionEx
class CheckColonyFilterCount : public ConditionEx
{
private:
    int     _currentCount;
    ///> type=Colony
    Colony  _colony;
public:
                CheckColonyFilterCount() {}
				CheckColonyFilterCount(PlayerType p_player, int p_filterTypeId, int p_operatorId, int p_value, const Colony& p_colony);
    int			CurrentCount() const { return _currentCount; }
    bool		Evaluate();
    void        Copy(IClonable* p_dest);
	bool		Consume(int p_amount) { return true; }
    //----------------------------------------------------------------------------------------------
    // Serialization
    string      TypeName()  { return "CheckColonyFilterCount"; }
    int         TypeSize()  { return sizeof(CheckColonyFilterCount); }
    UserObject* Prototype() { return new CheckColonyFilterCount; }
protected:
    void InitializeAddressesAux();
    //----------------------------------------------------------------------------------------------
};

#endif // CHECKCOLONYFILTERCOUNT_H_H    
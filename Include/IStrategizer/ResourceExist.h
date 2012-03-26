///> [Serializable]
#ifndef RESOURCEEXIST_H
#define RESOURCEEXIST_H

#include "ConditionEx.h"

///> class=ResourceExist
///> parent=ConditionEx
class ResourceExist : public ConditionEx
{
private:
	int _availableAmount;

public:
		        ResourceExist() {}
		        ResourceExist(PlayerType p_player, int p_resourceId, int p_amount);
	int		    AvailableAmount() const { return _availableAmount; }
	bool	    Evaluate();
    void        Copy(IClonable* p_dest);
	bool		Consume(int p_amount);
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "ResourceExist"; }
    int         TypeSize()  { return sizeof(ResourceExist); }
	UserObject* Prototype() { return new ResourceExist; }	
	//----------------------------------------------------------------------------------------------
};
#endif	// RESOURCEEXIST_H
///> [Serializable]
#ifndef RESEARCHDONE_H
#define RESEARCHDONE_H

#include "ConditionEx.h"

///> class=ResearchDone
///> parent=ConditionEx
class ResearchDone : public ConditionEx
{
public:
                    ResearchDone() {}
					ResearchDone(PlayerType p_player, ResearchType p_researchId) : ConditionEx(p_player, CONDEX_ResearchDone) { _conditionParameters[PARAM_UpgradeId] = p_researchId; }
	bool			Evaluate();
	bool			Consume(int p_amount) { return true; }
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "ResearchDone"; }
	int         TypeSize()  { return sizeof(ResearchDone); }
    UserObject* Prototype() { return new ResearchDone; }	
    //----------------------------------------------------------------------------------------------
};

#endif	// RESEARCHDONE_H
#include "GroundControlIM.h"
#include <cassert>
#include "RtsGame.h"
#include "GameEntity.h"
#include "GameType.h"
#include "Vector2.h"

using namespace IStrategizer;

const TCell PositiveInfluence = 1;
const TCell NegativeInfluence = -1;
const TCell NullInfluence = 0;

TCell GetInfluenceSign(PlayerType p_playerId)
{
	switch (p_playerId)
	{
	case PLAYER_Self:
		return PositiveInfluence;
	case PLAYER_Enemy:
		return NegativeInfluence;
	case PLAYER_Neutral:
	default:
		return NullInfluence;
	}
}
//////////////////////////////////////////////////////////////////////////
void GetInfluence(GameEntity *p_pGameObj, int &p_effectiveDistance, int &p_maxDistance, TCell &p_initValue)
{
	TCell			infSign = GetInfluenceSign((PlayerType)p_pGameObj->Attr(EOATTR_OwnerId));
	EntityClassType typeId;
	GameType		*pObjType = NULL;
	
	typeId = p_pGameObj->Type();
	pObjType = g_Game->GetEntityType(typeId);

	p_effectiveDistance = max(pObjType->Attr(ECATTR_GroundRange), pObjType->Attr(ECATTR_AirRange));
	p_maxDistance = pObjType->Attr(ECATTR_LineOfSight);
	// For non-attacking units (attack damage = 0) we consider their existence as influence in itself
	// We add 1 for all attack damages to take into account those non-attacking units
	// Also because we scale all attack damages the same, this has no effect on influence considerations
	// The relative influence should be conserved this way
	p_initValue = (pObjType->Attr(ECATTR_Attack) + 10) * infSign;
}
//////////////////////////////////////////////////////////////////////////
void StampObjField(InfluenceMap *p_pCaller, RegObjEntry *p_pObjEntry)
{
	GameEntity	*pGameObj = NULL;
	Vector2		currentPosition;
	Vector2		centerPosition;
	int			effectiveDistance;
	int			maxDistance;
	TCell		initValue;

	pGameObj = p_pCaller->GetObj(p_pObjEntry);
	assert(pGameObj);
	currentPosition.X = pGameObj->Attr(EOATTR_PosX);
	currentPosition.Y = pGameObj->Attr(EOATTR_PosY);

	// Optimization: we skip neutral units because they don't have influence
	if (p_pObjEntry->OwnerId == PLAYER_Neutral)
		return;

	p_pObjEntry->Stamped = true;
	p_pObjEntry->LastPosition = currentPosition;

	centerPosition.X = pGameObj->Attr(EOATTR_PosCenterX);
	centerPosition.Y = pGameObj->Attr(EOATTR_PosCenterY);
	
	GetInfluence(pGameObj, effectiveDistance, maxDistance, initValue);
	p_pCaller->StampInfluenceGradient(centerPosition, maxDistance, effectiveDistance, initValue);
}
//////////////////////////////////////////////////////////////////////////
void GroundControlIM::Update(unsigned p_gameCycle)
{
	ClearMap();
	ResetStats();
	ForEachObj(StampObjField);
}
//////////////////////////////////////////////////////////////////////////
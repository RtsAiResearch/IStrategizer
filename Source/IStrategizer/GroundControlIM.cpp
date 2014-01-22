#include "GroundControlIM.h"
#include <cassert>
#include "RtsGame.h"
#include "GameEntity.h"
#include "GameType.h"
#include "Vector2.h"

using namespace IStrategizer;

const TInfluence PositiveInfluence = 1;
const TInfluence NegativeInfluence = -1;
const TInfluence NullInfluence = 0;

TInfluence GetInfluenceSign(PlayerType p_playerId)
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
void GetInfluence(RtsGame& p_RtsGame, GameEntity *p_pGameObj, int &p_effectiveDistance, int &p_maxDistance, TInfluence &p_initValue)
{
    TInfluence infSign = GetInfluenceSign((PlayerType)p_pGameObj->Attr(EOATTR_OwnerId));
    EntityClassType typeId;
    GameType *pObjType = nullptr;
    
    typeId = p_pGameObj->Type();
    pObjType = p_RtsGame.GetEntityType(typeId);

    p_effectiveDistance = max(pObjType->Attr(ECATTR_GroundRange), pObjType->Attr(ECATTR_AirRange));
    p_maxDistance = pObjType->Attr(ECATTR_LineOfSight);
    // For non-attacking units (attack damage = 0) we consider their existence as influence in itself
    // We add 1 for all attack damages to take into account those non-attacking units
    // Also because we scale all attack damages the same, this has no effect on influence considerations
    // The relative influence should be conserved this way
    p_initValue = (pObjType->Attr(ECATTR_Attack) + 10) * infSign;
}
//////////////////////////////////////////////////////////////////////////
void StampObjField(RtsGame& p_RtsGame, InfluenceMap *p_Caller, RegObjEntry *p_ObjEntry)
{
    GameEntity *pGameObj = nullptr;
    Vector2 currentPosition;
    Vector2 centerPosition;
    int effectiveDistance;
    int maxDistance;
    TInfluence initValue;

    pGameObj = p_Caller->GetObj(p_RtsGame, p_ObjEntry);
    assert(pGameObj);
    currentPosition.X = pGameObj->Attr(EOATTR_PosX);
    currentPosition.Y = pGameObj->Attr(EOATTR_PosY);

    // Optimization: we skip neutral units because they don't have influence
    if (p_ObjEntry->OwnerId == PLAYER_Neutral)
        return;

    p_ObjEntry->Stamped = true;
    p_ObjEntry->LastPosition = currentPosition;

    centerPosition.X = pGameObj->Attr(EOATTR_PosCenterX);
    centerPosition.Y = pGameObj->Attr(EOATTR_PosCenterY);
    
    GetInfluence(p_RtsGame, pGameObj, effectiveDistance, maxDistance, initValue);
    p_Caller->StampInfluenceGradient(centerPosition, maxDistance, effectiveDistance, initValue);
}
//////////////////////////////////////////////////////////////////////////
void GroundControlIM::Update(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    ClearMap();
    ResetStats();
    ForEachObj(p_RtsGame, StampObjField);
}
//////////////////////////////////////////////////////////////////////////

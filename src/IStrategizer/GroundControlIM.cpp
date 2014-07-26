#include "GroundControlIM.h"
#include <cassert>
#include "RtsGame.h"
#include "GameEntity.h"
#include "GameType.h"
#include "Vector2.h"
#include "InfluenceMap.h"

using namespace IStrategizer;
using namespace std;

const TInfluence PositiveInfluence = 1;
const TInfluence NegativeInfluence = -1;
const TInfluence NullInfluence = 0;

std::unordered_map<EntityClassType, TInfluence> g_InfluenceCache;

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
void GetInfluence(GameEntity *p_pGameObj, int &effectiveDistance, int &maxDistance, TInfluence &infValue)
{
    TInfluence infSign = GetInfluenceSign((PlayerType)p_pGameObj->P(OP_OwnerId));
    EntityClassType typeId;

    if (g_InfluenceCache.count(typeId) == 0)
    {
        GameType *pObjType = nullptr;

        typeId = p_pGameObj->TypeId();
        pObjType = g_Game->GetEntityType(typeId);

        effectiveDistance = max(pObjType->P(TP_GroundRange), pObjType->P(TP_AirRange));
        maxDistance = pObjType->P(TP_LineOfSight);
        // For non-attacking units (attack damage = 0) we consider their existence as influence in itself
        // We add 10 for all attack damages to take into account those non-attacking units
        // Also because we scale all attack damages the same, this has no effect on influence considerations
        // The relative influence should be conserved this way
        int attackDmg = max(pObjType->P(TP_GroundAttack), pObjType->P(TP_AirAttack));
        infValue = (attackDmg + 10);
        g_InfluenceCache[typeId] = infValue;
    }
    else
    {
        infValue = g_InfluenceCache[typeId];
    }

    infValue *= infSign;
}
//////////////////////////////////////////////////////////////////////////
void StampObjField(InfluenceMap *p_pCaller, RegObjEntry *p_pObjEntry)
{
    GameEntity *pGameObj = nullptr;
    Vector2 currentPosition;
    Vector2 centerPosition;
    int effectiveDistance;
    int maxDistance;
    TInfluence initValue;

    pGameObj = p_pCaller->GetObj(p_pObjEntry);
    _ASSERTE(pGameObj);
    currentPosition.X = pGameObj->P(OP_Left);
    currentPosition.Y = pGameObj->P(OP_Top);

    // Optimization: we skip neutral units because they don't have influence
    if (p_pObjEntry->OwnerId == PLAYER_Neutral)
        return;

    p_pObjEntry->Stamped = true;
    p_pObjEntry->LastPosition = currentPosition;

    centerPosition.X = pGameObj->P(OP_PosCenterX);
    centerPosition.Y = pGameObj->P(OP_PosCenterY);
    
    GetInfluence(pGameObj, effectiveDistance, maxDistance, initValue);
    p_pCaller->StampInfluenceGradient(centerPosition, maxDistance, effectiveDistance, initValue);
}
//////////////////////////////////////////////////////////////////////////
void GroundControlIM::Update(const WorldClock& p_clock)
{
    ClearMap();
    ResetStats();
    ForEachObj(StampObjField);
}
//////////////////////////////////////////////////////////////////////////
TInfluence GroundControlIM::GetCellInfluenceFromWorldPosition(const Vector2 p_worldPosition)
{
    Vector2 tempGridPostion;
    FromWorldToGrid(p_worldPosition, tempGridPostion);
    return Map()[tempGridPostion.Y * GridWidth() + tempGridPostion.X].Inf;
}

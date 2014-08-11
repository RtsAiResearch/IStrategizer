#include "OccupanceDataIM.h"
#include <algorithm>
#include "RtsGame.h"
#include "WorldMap.h"
#include "GameEntity.h"
#include "Vector2.h"
#include "GameType.h"
#include "GamePlayer.h"

using namespace IStrategizer;
using namespace std;

const TInfluence PositiveInfluence = 1;
const TInfluence NegativeInfluence = -1;
const TInfluence NullInfluence = 0;

//////////////////////////////////////////////////////////////////////////
void UnstampDirtyObj(InfluenceMap *pCaller, RegObjEntry *pObjEntry)
{
    GameEntity *pGameObj = nullptr;
    Vector2 currentPosition;

    pGameObj = pCaller->GetObj(pObjEntry);
    _ASSERTE(pGameObj);
    currentPosition.X = pGameObj->P(OP_Left);
    currentPosition.Y = pGameObj->P(OP_Top);

    auto stampWidth = pObjEntry->ObjWidth;
    auto stampHeight = pObjEntry->ObjHeight;

    if (pObjEntry->IsAllSidePadded)
    {
        currentPosition.X -= pObjEntry->PaddingSize;
        if (currentPosition.X < 0)
        {
            currentPosition.X = 0;
            stampWidth -= pObjEntry->PaddingSize;
        }

        currentPosition.Y -= pObjEntry->PaddingSize;
        if (currentPosition.Y < 0)
        {
            currentPosition.Y = 0;
            stampHeight -= pObjEntry->PaddingSize;
        }
    }

    // If not dirty, then skip
    if (currentPosition == pObjEntry->LastPosition)
        return;

    // Unstamp dirty object
    if (pObjEntry->Stamped)
        pCaller->StampInfluenceShape(pObjEntry->LastPosition, stampWidth, stampHeight, NegativeInfluence);
}
//////////////////////////////////////////////////////////////////////////
void StampNonDirtyObj(InfluenceMap *pCaller, RegObjEntry *pObjEntry)
{
    GameEntity *pGameObj = nullptr;
    Vector2 currentPosition;

    pGameObj = pCaller->GetObj(pObjEntry);
    _ASSERTE(pGameObj);

    currentPosition.X = pGameObj->P(OP_Left);
    currentPosition.Y = pGameObj->P(OP_Top);
    auto stampWidth = pObjEntry->ObjWidth;
    auto stampHeight = pObjEntry->ObjHeight;

    // Apply padding and make sure that the new coordinates is within map
    // and apply the required trimming for width and height
    if (pObjEntry->IsAllSidePadded)
    {
        currentPosition.X -= pObjEntry->PaddingSize;
        if (currentPosition.X < 0)
        {
            currentPosition.X = 0;
            stampWidth -= pObjEntry->PaddingSize;
        }

        currentPosition.Y -= pObjEntry->PaddingSize;
        if (currentPosition.Y < 0)
        {
            currentPosition.Y = 0;
            stampHeight -= pObjEntry->PaddingSize;
        }
    }

    // If not dirty, then skip
    // Note that objects added for the first time will have an invalid position and is considered as dirty
    if (currentPosition == pObjEntry->LastPosition)
        return;

    pObjEntry->Stamped = true;
    pObjEntry->LastPosition = currentPosition;
    pCaller->StampInfluenceShape(pObjEntry->LastPosition, stampWidth, stampHeight, PositiveInfluence);
}
//////////////////////////////////////////////////////////////////////////
void OccupanceDataIM::Update(const WorldClock& p_clock)
{
    if (m_registeredObjects.empty())
        return;

    // Using the dirty rectangles techniques used in Graphics, we keep unchanged objects and
    // unstamp dirty objects
    ForEachObj(UnstampDirtyObj);
    ForEachObj(StampNonDirtyObj);
}
//////////////////////////////////////////////////////////////////////////
void OccupanceDataIM::UnregisterGameObj(TID objId)
{
    if (m_registeredObjects.empty())
        return;

    if (m_registeredObjects.count(objId) > 0)
    {
        auto pObjEntry = m_registeredObjects[objId];

        if (pObjEntry->Stamped)
            StampInfluenceShape(pObjEntry->LastPosition, pObjEntry->ObjWidth, pObjEntry->ObjHeight, NegativeInfluence);

        m_registeredObjects.erase(objId);
        delete pObjEntry;
    }
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::OccupancePredicate(unsigned worldX, unsigned worldY, TCell* pCell, void *pParam)
{
    bool stopSearch = false;

    _ASSERTE(pParam);
    bool *pAllCellsFree = (bool*)pParam;

    bool isTileBuildable = g_Game->Map()->CanBuildHere(Vector2(worldX, worldY));

    _ASSERTE(pCell);
    if (pCell->Inf != NullInfluence ||
        pCell->Data != CELL_Free ||
        !isTileBuildable)
    {
        stopSearch = true;
        *pAllCellsFree = false;
    }

    return stopSearch;
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::IsAreaOccupied(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight)
{
    bool allCellsFree = true;

    ForEachCellInArea(p_areaPos, p_areaWidth, p_areaHeight, OccupancePredicate, &allCellsFree);

    return !allCellsFree;
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::ReservePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_pCell, void *p_pParam)
{
    bool stopSearch = false;

    _ASSERTE(p_pParam);
    bool *pReserveOk = (bool*)p_pParam;

    _ASSERTE(p_pCell);
    if (p_pCell->Data == CELL_Free)
    {
        p_pCell->Data = CELL_Reserved;
    }
    else
    {
        stopSearch = true;
        *pReserveOk = false;
    }

    return stopSearch;
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::ReserveArea(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight)
{
    bool reserveOk = true;

    ForEachCellInArea(p_areaPos, p_areaWidth, p_areaHeight, ReservePredicate, &reserveOk);

    return reserveOk;
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::FreePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_pCell, void *p_pParam)
{
    bool stopSearch = false;

    _ASSERTE(p_pParam);
    bool *pFreeOk = (bool*)p_pParam;

    _ASSERTE(p_pCell);
    if (p_pCell->Data == CELL_Reserved)
    {
        p_pCell->Data = CELL_Free;
    }
    else
    {
        stopSearch = true;
        *pFreeOk = false;
    }

    return stopSearch;
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::FreeArea(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight)
{
    bool freeOk = true;

    ForEachCellInArea(p_areaPos, p_areaWidth, p_areaHeight, FreePredicate, &freeOk);

    return freeOk;
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::CanBuildHere(Vector2 p_worldPos, int p_buildingWidth, int p_buildingHeight, EntityClassType p_buildingType)
{
    return !this->IsAreaOccupied(p_worldPos, p_buildingWidth, p_buildingHeight) &&
        g_Game->Map()->CanBuildHere(p_worldPos, p_buildingType);
}
//////////////////////////////////////////////////////////////////////////
void OccupanceDataIM::RegisterGameObj(TID objId, PlayerType ownerId)
{
    if (m_registeredObjects.count(objId) == 0)
        UnregisterGameObj(objId);

    RegObjEntry *pNewObj = new RegObjEntry;
    GameEntity *pGameObj = nullptr;
    GameType *pObjType = nullptr;

    pNewObj->ObjId = objId;
    pNewObj->OwnerId = ownerId;
    pNewObj->Stamped = false;

    pGameObj = g_Game->GetPlayer(ownerId)->GetEntity(objId);
    _ASSERTE(pGameObj);
    pNewObj->LastPosition = Vector2(-1, -1);

    pObjType = g_Game->GetEntityType((EntityClassType)pGameObj->TypeId());
    _ASSERTE(pObjType);

    pNewObj->ObjWidth = pObjType->P(TP_Width) + pObjType->P(TP_BuildingExpansionIncrement);
    pNewObj->ObjHeight = pObjType->P(TP_Height);

    if ((pNewObj->OwnerId == PLAYER_Self &&
        (pObjType->P(TP_IsResoureDepot) ||
        g_Game->Self()->Race()->GetResourceSource(RESOURCE_Secondary) == pGameObj->TypeId())) ||
        pObjType->P(TP_IsSecondaryResource) ||
        pObjType->P(TP_IsPrimaryResource))
    {
        pNewObj->IsAllSidePadded = true;
        pNewObj->PaddingSize = m_cellSide;

        pNewObj->ObjWidth += (2 * pNewObj->PaddingSize);
        pNewObj->ObjHeight += (2 * pNewObj->PaddingSize);
    }

    m_registeredObjects[objId] = pNewObj;
}
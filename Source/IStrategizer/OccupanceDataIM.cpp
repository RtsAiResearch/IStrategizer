#include "OccupanceDataIM.h"
#include <algorithm>
#include "RtsGame.h"
#include "GameEntity.h"
#include "Vector2.h"

using namespace IStrategizer;
using namespace std;

const TInfluence PositiveInfluence = 1;
const TInfluence NegativeInfluence = -1;
const TInfluence nullptrInfluence = 0;

//////////////////////////////////////////////////////////////////////////
void UnstampDirtyObj(RtsGame& p_RtsGame, InfluenceMap *p_Caller, RegObjEntry *p_ObjEntry)
{
    GameEntity *pGameObj = nullptr;
    Vector2 currentPosition;

    pGameObj = p_Caller->GetObj(p_RtsGame, p_ObjEntry);
    assert(pGameObj);
    currentPosition.X = pGameObj->Attr(EOATTR_PosX);
    currentPosition.Y = pGameObj->Attr(EOATTR_PosY);
    
    // If not dirty, then skip
    if (currentPosition == p_ObjEntry->LastPosition)
        return;

    // Unstamp dirty object
    if(p_ObjEntry->Stamped)
        p_Caller->StampInfluenceShape(p_ObjEntry->LastPosition, p_ObjEntry->ObjWidth, p_ObjEntry->ObjHeight, NegativeInfluence);
}
//////////////////////////////////////////////////////////////////////////
void StampNonDirtyObj(RtsGame& p_RtsGame, InfluenceMap *p_Caller, RegObjEntry *p_ObjEntry)
{
    GameEntity *pGameObj = nullptr;
    Vector2 currentPosition;

    pGameObj = p_Caller->GetObj(p_RtsGame, p_ObjEntry);
    assert(pGameObj);
    currentPosition.X = pGameObj->Attr(EOATTR_PosX);
    currentPosition.Y = pGameObj->Attr(EOATTR_PosY);

    // If not dirty, then skip
    // Note that objects added for the first time will have an invalid position and is considered as dirty
    if (currentPosition == p_ObjEntry->LastPosition)
        return;

    p_ObjEntry->Stamped = true;
    p_ObjEntry->LastPosition = currentPosition;
    p_Caller->StampInfluenceShape(p_ObjEntry->LastPosition, p_ObjEntry->ObjWidth, p_ObjEntry->ObjHeight, PositiveInfluence);
}
//////////////////////////////////////////////////////////////////////////
void OccupanceDataIM::Update(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    if (m_registeredObjects.empty())
        return;

    // Using the dirty rectangles techniques used in Graphics, we keep unchanged objects and
    // unstamp dirty objects
    ForEachObj(p_RtsGame, UnstampDirtyObj);
    ForEachObj(p_RtsGame, StampNonDirtyObj);
}
//////////////////////////////////////////////////////////////////////////
void OccupanceDataIM::UnregisterGameObj(TID p_objId)
{
    if (m_registeredObjects.empty())
        return;

    RegObjectList::iterator objItr;
    RegObjEntry *pObjEntry;

    for (objItr = m_registeredObjects.begin(); objItr != m_registeredObjects.end(); ++objItr)
    {
        pObjEntry = *objItr;

        if (pObjEntry->ObjId == p_objId)
        {
            if (pObjEntry->Stamped)
                StampInfluenceShape(pObjEntry->LastPosition, pObjEntry->ObjWidth, pObjEntry->ObjHeight, NegativeInfluence);

            m_registeredObjects.erase(objItr);
            delete pObjEntry;
            return;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::OccupancePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_Cell, void *p_Param)
{
    bool stopSearch = false;

    assert(p_Param);
    bool *pAllCellsFree = (bool*)p_Param;

    assert(p_Cell);
    if (p_Cell->Inf != nullptrInfluence || p_Cell->Data != CELL_Free)
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
bool OccupanceDataIM::ReservePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_Cell, void *p_Param)
{
    bool stopSearch = false;

    assert(p_Param);
    bool *pReserveOk = (bool*)p_Param;

    assert(p_Cell);
    if (p_Cell->Data == CELL_Free)
    {
        p_Cell->Data = CELL_Reserved;
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
bool OccupanceDataIM::FreePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_Cell, void *p_Param)
{
    bool stopSearch = false;

    assert(p_Param);
    bool *pFreeOk = (bool*)p_Param;

    assert(p_Cell);
    if (p_Cell->Data == CELL_Reserved)
    {
        p_Cell->Data = CELL_Free;
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

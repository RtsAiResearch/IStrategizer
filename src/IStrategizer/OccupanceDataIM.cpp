#include "OccupanceDataIM.h"
#include <algorithm>
#include "RtsGame.h"
#include "WorldMap.h"
#include "GameEntity.h"
#include "Vector2.h"

using namespace IStrategizer;
using namespace std;

const TInfluence PositiveInfluence = 1;
const TInfluence NegativeInfluence = -1;
const TInfluence nullptrInfluence = 0;

//////////////////////////////////////////////////////////////////////////
void UnstampDirtyObj(InfluenceMap *p_pCaller, RegObjEntry *p_pObjEntry)
{
    GameEntity *pGameObj = nullptr;
    Vector2 currentPosition;

    pGameObj = p_pCaller->GetObj(p_pObjEntry);
    _ASSERTE(pGameObj);
    currentPosition.X = pGameObj->Attr(EOATTR_Left);
    currentPosition.Y = pGameObj->Attr(EOATTR_Top);
    
    // If not dirty, then skip
    if (currentPosition == p_pObjEntry->LastPosition)
        return;

    // Unstamp dirty object
    if(p_pObjEntry->Stamped)
        p_pCaller->StampInfluenceShape(p_pObjEntry->LastPosition, p_pObjEntry->ObjWidth, p_pObjEntry->ObjHeight, NegativeInfluence);
}
//////////////////////////////////////////////////////////////////////////
void StampNonDirtyObj(InfluenceMap *p_pCaller, RegObjEntry *p_pObjEntry)
{
    GameEntity *pGameObj = nullptr;
    Vector2 currentPosition;

    pGameObj = p_pCaller->GetObj(p_pObjEntry);
    _ASSERTE(pGameObj);
    currentPosition.X = pGameObj->Attr(EOATTR_Left);
    currentPosition.Y = pGameObj->Attr(EOATTR_Top);

    // If not dirty, then skip
    // Note that objects added for the first time will have an invalid position and is considered as dirty
    if (currentPosition == p_pObjEntry->LastPosition)
        return;

    p_pObjEntry->Stamped = true;
    p_pObjEntry->LastPosition = currentPosition;
    p_pCaller->StampInfluenceShape(p_pObjEntry->LastPosition, p_pObjEntry->ObjWidth, p_pObjEntry->ObjHeight, PositiveInfluence);
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
bool OccupanceDataIM::OccupancePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_pCell, void *p_pParam)
{
    bool stopSearch = false;

    _ASSERTE(p_pParam);
    bool *pAllCellsFree = (bool*)p_pParam;

    _ASSERTE(p_pCell);
    if (p_pCell->Inf != nullptrInfluence || p_pCell->Data != CELL_Free)
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

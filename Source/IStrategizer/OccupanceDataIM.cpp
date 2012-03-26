#include "OccupanceDataIM.h"
#include <algorithm>
#include "RtsGame.h"
#include "GameEntity.h"
#include "Vector2.h"

using namespace IStrategizer;
using namespace MetaData;
using namespace std;

const TCell PositiveInfluence = 1;
const TCell NegativeInfluence = -1;
const TCell NullInfluence = 0;

//////////////////////////////////////////////////////////////////////////
void UnstampDirtyObj(InfluenceMap *p_pCaller, RegObjEntry *p_pObjEntry)
{
	GameEntity *pGameObj = NULL;
	Vector2		currentPosition;

	pGameObj = p_pCaller->GetObj(p_pObjEntry);
	assert(pGameObj);
	currentPosition.X = pGameObj->Attr(EOATTR_PosX);
	currentPosition.Y = pGameObj->Attr(EOATTR_PosY);
	
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
	GameEntity *pGameObj = NULL;
	Vector2 currentPosition;

	pGameObj = p_pCaller->GetObj(p_pObjEntry);
	assert(pGameObj);
	currentPosition.X = pGameObj->Attr(EOATTR_PosX);
	currentPosition.Y = pGameObj->Attr(EOATTR_PosY);

	// If not dirty, then skip
	// Note that objects added for the first time will have an invalid position and is considered as dirty
	if (currentPosition == p_pObjEntry->LastPosition)
		return;

	p_pObjEntry->Stamped = true;
	p_pObjEntry->LastPosition = currentPosition;
	p_pCaller->StampInfluenceShape(p_pObjEntry->LastPosition, p_pObjEntry->ObjWidth, p_pObjEntry->ObjHeight, PositiveInfluence);
}
//////////////////////////////////////////////////////////////////////////
void OccupanceDataIM::Update(unsigned p_gameCycle)
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
	bool stopSearch;
	assert(p_pParam);
	bool &allCellsFree = *((bool*)p_pParam);

	assert(p_pCell);
	allCellsFree &= (*p_pCell == 0);
	stopSearch = !allCellsFree;

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
	bool stopSearch;
	assert(p_pParam);
	bool &allCellsFree = *((bool*)p_pParam);

	assert(p_pCell);
	allCellsFree &= (*p_pCell == 0);
	
	if (allCellsFree)
	{
		*p_pCell = NegativeInfluence;
	}

	stopSearch = !allCellsFree;

	return stopSearch;
}
//////////////////////////////////////////////////////////////////////////
bool OccupanceDataIM::ReserveArea(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight)
{
	bool allCellsFree = true;

	ForEachCellInArea(p_areaPos, p_areaWidth, p_areaHeight, ReservePredicate, &allCellsFree);

	return allCellsFree;
}
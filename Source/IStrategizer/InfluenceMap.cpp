#include "InfluenceMap.h"
#include <algorithm>
#include <cassert>
#include <cmath>

#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "GameType.h"

using namespace IStrategizer;
using namespace IStrategizer;
using namespace std;

InfluenceMap::~InfluenceMap()
{
    Reset();
	delete m_pMap;
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::Init(int p_cellWidth, int p_cellHeight, int p_worldWidth, int p_worldHeight)
{
    m_cellSide = p_cellWidth;
    m_cellSide = p_cellHeight;
    m_worldWidth = p_worldWidth;
    m_worldHeight = p_worldHeight;
	m_gridWidth = m_worldWidth / m_cellSide;
	m_gridHeight = m_worldHeight / m_cellSide;
	m_numCells = m_gridWidth  * m_gridHeight;

	m_pMap = new TCell[m_numCells];
	ClearMap();
}
//////////////////////////////////////////////////////////////////////////
void DeleteObjCallback(RegObjEntry* object)
{
    delete object;
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::Reset()
{
    ClearMap();

    if (m_registeredObjects.empty())
        return;

    for_each(m_registeredObjects.begin(), m_registeredObjects.end(), DeleteObjCallback);
    m_registeredObjects.clear();
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::RegisterGameObj(TID p_objId, PlayerType p_ownerId)
{
    RegObjEntry	*pNewObj = new RegObjEntry;
	GameEntity	*pGameObj = NULL;
	GameType	*pObjType = NULL;

    pNewObj->ObjId = p_objId;
	pNewObj->OwnerId = p_ownerId;
	pNewObj->Stamped = false;

	pGameObj = g_Game->GetPlayer(p_ownerId)->GetEntity(p_objId);
	assert(pGameObj);
	pNewObj->LastPosition = Vector2::Null();

	pObjType = g_Game->GetEntityType((EntityClassType)pGameObj->Type());
	assert(pObjType);
	pNewObj->ObjWidth = pObjType->Attr(ECATTR_Width);
	pNewObj->ObjHeight = pObjType->Attr(ECATTR_Height);

    m_registeredObjects.push_back(pNewObj);
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::UnregisterGameObj(TID p_objId)
{
    if (m_registeredObjects.empty())
        return;

    RegObjectList::iterator objItr;
	RegObjEntry* pObjEntry;

    for (objItr = m_registeredObjects.begin(); objItr != m_registeredObjects.end(); ++objItr)
    {
        pObjEntry = *objItr;

        if (pObjEntry->ObjId == p_objId)
        {
            m_registeredObjects.erase(objItr);
            delete pObjEntry;
            return;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::StampInfluenceShape(Vector2& p_startPosition, int p_width, int p_height, TCell p_value)
{
    int		gridX = (int)((float)p_startPosition.X / m_cellSide);
    int		gridY = (int)((float)p_startPosition.Y / m_cellSide);
    int		gridWidth = max((int)(ceil((float)p_width / m_cellSide)), 1);
	int		gridHeight = max((int)(ceil((float)p_height / m_cellSide)), 1);
	int		endX = min(gridX + gridWidth, m_gridWidth);
	int		endY = min(gridY + gridHeight, m_gridHeight);
	int		idx;
	TCell	cellInf;

    for(int y = gridY; y < endY; ++y)
    {
        for(int x = gridX; x < endX; ++x)
        {
			idx = y * m_gridWidth + x;

			if (idx < m_numCells)
			{
				m_pMap[idx] += p_value;

				cellInf = m_pMap[idx];

				if (cellInf > m_statistics.MaxInf)
				{
					m_statistics.MaxInfGridPos.X = x;
					m_statistics.MaxInfGridPos.Y = y;
					m_statistics.MaxInf = cellInf;
				}
				else if (cellInf < m_statistics.MinInf)
				{
					m_statistics.MinInfGridPos.X = x;
					m_statistics.MinInfGridPos.Y = y;
					m_statistics.MinInf = cellInf;
				}
			}
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::StampInfluenceGradient(Vector2& p_centerPosition, int p_fastFalloffDistance, int p_slowFalloffDistance, TCell p_initValue)
{
	const float	fastFalloffPercentage = 0.90f;
	const float slowFalloffPercentage = 1.0f;

	// The initial value falloff should fall slowly and the start to drop fast
	// The fast falloff circle (outer circle) should contain the slow falloff circle (inner circle)
	// We will approximate circles with squares as trade off between optimization and optimality
	if (p_fastFalloffDistance < p_slowFalloffDistance)
		swap(p_fastFalloffDistance, p_slowFalloffDistance);

	int		centerX = (int)((float)p_centerPosition.X / m_cellSide);
	int		centerY = (int)((float)p_centerPosition.Y / m_cellSide);
	int		outerHalfSide = max((int)(ceil((float)p_fastFalloffDistance / m_cellSide)), 1);
	int		innerHalfSide = max((int)(ceil((float)p_slowFalloffDistance / m_cellSide)), 1);
	int		outerLeft = max(centerX - outerHalfSide, 0);
	int		outerTop = max(centerY - outerHalfSide, 0);
	int		outerRight = min(centerX + outerHalfSide, m_gridWidth - 1);
	int		outerBottom = min(centerY + outerHalfSide, m_gridHeight - 1);
	int		innerLeft = max(centerX - innerHalfSide, 0);
	int		innerTop = max(centerY - innerHalfSide, 0);
	int		innerRight = min(centerX + innerHalfSide, m_gridWidth - 1);
	int		innerBottom = min(centerY + innerHalfSide, m_gridHeight - 1);
	int		maxManhattanDistance = outerHalfSide;
	float	curFalloffPercentage;
	int		curManhattanDistance;
	int		idx;
	TCell	cellInf;

	for(int y = outerTop; y <= outerBottom; ++y)
	{
		for(int x = outerLeft; x <= outerRight; ++x)
		{
			idx = y * m_gridWidth + x;

			cellInf = m_pMap[idx];
			curManhattanDistance = abs(y - centerY) + abs(x - centerX);

			if (curManhattanDistance > maxManhattanDistance)
				continue;
			else
				curFalloffPercentage = 1.0f - ((float)curManhattanDistance / (float)maxManhattanDistance);

			// We are directly on the center, assign initial value as our influence
			if (x == centerX && y == centerY)
				cellInf += p_initValue;
			// We are inside the inner circle
			// This should be a slow drop off period
			if (x >= innerLeft && x <= innerRight &&
				y >= innerTop && y <= innerBottom)
			{
				cellInf += (int)(slowFalloffPercentage * curFalloffPercentage * (float)p_initValue);
			}
			// We are outside the inner circle and inside the outer circle
			// This should be a fast drop off period
			else
			{
				cellInf += (int)(fastFalloffPercentage * curFalloffPercentage * (float)p_initValue);
			}

			m_pMap[idx] = cellInf;

			if (cellInf > m_statistics.MaxInf)
			{
				m_statistics.MaxInfGridPos.X = x;
				m_statistics.MaxInfGridPos.Y = y;
				m_statistics.MaxInf = cellInf;
			}
			else if (cellInf < m_statistics.MinInf)
			{
				m_statistics.MinInfGridPos.X = x;
				m_statistics.MinInfGridPos.Y = y;
				m_statistics.MinInf = cellInf;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::FromWorldToGrid(const Vector2 &p_worldPosition, Vector2 &p_gridPosition)
{
	p_gridPosition.X = (int)((float)p_worldPosition.X / m_cellSide);
	p_gridPosition.Y = (int)((float)p_worldPosition.Y / m_cellSide);
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::FromGridToWorld(const Vector2 &p_gridPosition, Vector2 &p_worldPosition)
{
	p_worldPosition.X = (int)((float)p_gridPosition.X / m_cellSide);
	p_worldPosition.Y = (int)((float)p_gridPosition.Y / m_cellSide);
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::ClearMap()
{
	memset(m_pMap, 0, m_numCells * sizeof(TCell));
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::ResetStats()
{
	memset(&m_statistics, 0, sizeof(IMStatistics));
}
//////////////////////////////////////////////////////////////////////////
TCell InfluenceMap::SumInfluenceShape(Vector2& p_startPosition, int p_width, int p_height)
{
    TCell sum = TCell();
	int gridX = (int)((float)p_startPosition.X / m_cellSide);
	int gridY = (int)((float)p_startPosition.Y / m_cellSide);
	int gridWidth = max((int)(ceil((float)p_width / m_cellSide)), 1);
	int gridHeight = max((int)(ceil((float)p_height / m_cellSide)), 1);
	int endX = min(gridX + gridWidth, m_gridWidth);
	int endY = min(gridY + gridHeight, m_gridHeight);
	int idx;

	for(int y = gridY; y < endY; ++y)
	{
		for(int x = gridX; x < endX; ++x)
		{
			idx = y * m_gridWidth + x;

			if (idx < m_numCells)
				sum += m_pMap[idx];
		}
	}

	return sum;
}
//////////////////////////////////////////////////////////////////////////
GameEntity* InfluenceMap::GetObj(RegObjEntry* p_pObjEntry)
{
	GameEntity	*pGameObj = NULL;
	GamePlayer	*pPlayer = NULL;

	pPlayer = g_Game->GetPlayer(p_pObjEntry->OwnerId);
	assert(pPlayer);

	pGameObj = pPlayer->GetEntity(p_pObjEntry->ObjId);
	assert(pGameObj);

	return pGameObj;
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::ForEachObj(RegObjCallback p_pfnCallback)
{
	RegObjectList::iterator objItr;

	for (objItr = m_registeredObjects.begin(); objItr != m_registeredObjects.end(); ++objItr)
	{
		p_pfnCallback(this, *objItr);
	}
}
//////////////////////////////////////////////////////////////////////////
bool InfluenceMap::InBound(int p_gridX, int p_gridY)
{
	return p_gridX >= 0 && p_gridX < m_gridWidth &&
		p_gridY >= 0 && p_gridY < m_gridHeight;
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::SpiralMove(const Vector2& p_spiralStart, unsigned p_radiusLength, SpiralMovePredicate p_pfnPred, void *p_pParam)
{
	enum CycleStep { DOWN = 0, LEFT, UP, RIGHT };

	int			currentStepType;
	int			searchLimit[4] = { 0 };
	bool		searchExhausted[4] = { false };
	int			currentX, currentY;
	unsigned	currentMoveSteps;
	unsigned	targetMoveSteps;
	int			toggleStep1, toggleStep2;

	// Convert from world space to grid space
	currentX = (int)((float)p_spiralStart.X / m_cellSide);
	currentY = (int)((float)p_spiralStart.Y / m_cellSide);
	p_radiusLength /= m_cellSide;

	searchLimit[UP] = currentY - p_radiusLength;
	searchLimit[DOWN] = currentY + p_radiusLength;
	searchLimit[LEFT] = currentX - p_radiusLength;
	searchLimit[RIGHT] = currentX + p_radiusLength;

	currentStepType = DOWN;
	toggleStep1 = currentStepType;
	toggleStep2 = (currentStepType + 2) % 4;
	targetMoveSteps = 1;
	currentMoveSteps = 0;

	while (true)
	{
		switch (currentStepType)
		{
		case DOWN:
			++currentY;
			searchExhausted[DOWN] = (currentY > searchLimit[DOWN]);
			break;
		case LEFT:
			--currentX;
			searchExhausted[LEFT] = (currentX < searchLimit[LEFT]);
			break;
		case UP:
			--currentY;
			searchExhausted[UP] = (currentY < searchLimit[UP]);
			break;
		case RIGHT:
			++currentX;
			searchExhausted[RIGHT] = (currentX > searchLimit[RIGHT]);
			break;
		}

		if (searchExhausted[DOWN] ||
			searchExhausted[LEFT] ||
			searchExhausted[UP] ||
			searchExhausted[RIGHT])
			break;

		if (InBound(currentX, currentY))
		{
			if(p_pfnPred(currentX * m_cellSide, currentY * m_cellSide, &m_pMap[currentY * m_gridWidth + currentX], p_pParam))
				break;
		}

		// Consume a step
		++currentMoveSteps;

		// Exhausted our steps for the current step type ?
		if (currentMoveSteps == targetMoveSteps)
		{
			// Get the next step type in a cycle fashion
			++currentStepType;
			currentStepType %= 4;
			currentMoveSteps = 0;

			if (currentStepType == toggleStep1 || currentStepType == toggleStep2)
			{
				// Increase our next target steps to move
				++targetMoveSteps;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void InfluenceMap::ForEachCellInArea(const Vector2& p_areaStartPos, int p_areaWidth, int p_areaHeight, CellPredicate p_pfnPred, void *p_pParam)
{
	TCell sum = TCell();
	int gridX = (int)((float)p_areaStartPos.X / m_cellSide);
	int gridY = (int)((float)p_areaStartPos.Y / m_cellSide);
	int gridWidth = max((int)(ceil((float)p_areaWidth / m_cellSide)), 1);
	int gridHeight = max((int)(ceil((float)p_areaHeight / m_cellSide)), 1);
	int endX = min(gridX + gridWidth, m_gridWidth);
	int endY = min(gridY + gridHeight, m_gridHeight);
	int idx;

	for(int y = gridY; y < endY; ++y)
	{
		for(int x = gridX; x < endX; ++x)
		{
			idx = y * m_gridWidth + x;

			if (idx < m_numCells)
			{
				if(p_pfnPred(x * m_cellSide, y * m_cellSide, &m_pMap[idx], p_pParam))
					return;
			}
		}
	}
}


#ifndef INFLUENCEMAP_H
#define INFLUENCEMAP_H

#include <list>
#include "EngineData.h"
#include "Vector2.h"
#include "IMSystemManager.h"

namespace IStrategizer
{
	using namespace MetaData;

	class GameEntity;
	class InfluenceMap;
	struct RegObjEntry;

	typedef int TCell;
	typedef std::list<RegObjEntry*> RegObjectList;
	typedef void (*RegObjCallback)(InfluenceMap *p_pCaller, RegObjEntry *p_pObjEntry);

	// This callback is called for each valid cell in a rectangular area
	// Return true if the path move should stop, otherwise return false to continue in the path
	typedef bool (*CellPredicate)(unsigned p_worldX, unsigned p_worldY, TCell* p_pCell, void *p_pParam);

	// This callback is called for each valid cell in the spiral path
	// Return true if the path move should stop, otherwise return false to continue in the path
	// The position passed is in world coordinates not the IM
	typedef bool (*SpiralMovePredicate)(unsigned p_worldX, unsigned p_worldY, const TCell* p_pCell, void *p_pParam);


	struct RegObjEntry
	{
		TID				ObjId;
		PlayerType		OwnerId;
		Vector2			LastPosition;
		bool			Stamped;
		int				ObjWidth;
		int				ObjHeight;
		RegObjEntry() : ObjId(TID()), LastPosition(Vector2::Null()), Stamped(false) {}
	};

	struct IMCell
	{
		void	*pHead;
		TCell	Influence;

		IMCell() : Influence(TCell()), pHead(NULL) {}
		IMCell(TCell p_influence) : Influence(p_influence), pHead(NULL) {}

		operator TCell&() { return Influence; }
	};

	struct IMStatistics
	{
		TCell	MaxInf;
		Vector2	MaxInfGridPos;
		TCell	MinInf;
		Vector2	MinInfGridPos;

		IMStatistics() : MaxInf(TCell()), MinInf(TCell()), 
			MaxInfGridPos(Vector2::Zero()), MinInfGridPos(Vector2::Zero()) {}
	};

	class InfluenceMap
	{
	public:
		InfluenceMap(IMType p_typeId) : m_typeId(p_typeId) {}
		virtual ~InfluenceMap();
		virtual void Update(unsigned p_gameCycle) {}
		virtual void Init(int p_cellWidth, int p_cellHeight, int p_worldWidth, int p_worldHeight);
		virtual void Reset();
		virtual void RegisterGameObj(TID p_objId, PlayerType p_ownerId);
		virtual void UnregisterGameObj(TID p_objectId);
		virtual void StampInfluenceShape(Vector2& p_startPosition, int p_width, int p_height, TCell p_value);
		virtual void StampInfluenceGradient(Vector2& p_centerPosition, int p_fastFalloffDistance, int p_slowFalloffDistance, TCell p_initValue);
		TCell  SumInfluenceShape(Vector2& p_startPosition, int p_width, int p_height);
		const IStrategizer::RegObjectList &RegisteredObjects() const { return m_registeredObjects; }
		IStrategizer::IMType TypeId() const { return m_typeId; }
		int CellSide() const { return m_cellSide; }
		int WorldWidth() const { return m_worldWidth; }
		int WorldHeight() const { return m_worldHeight; }
		int GridWidth() const { return m_gridWidth; }
		int GridHeight() const { return m_gridHeight; }
		const IStrategizer::IMStatistics &Statistics() const { return m_statistics; }
		static GameEntity* GetObj(RegObjEntry* p_pObjEntry);
		const TCell* Map() const { return m_pMap; }
		void SpiralMove(const Vector2& p_spiralStart, unsigned p_radiusLength, SpiralMovePredicate p_pfnPred, void *p_pParam);
		void ForEachCellInArea(const Vector2& p_areaStartPos, int p_areaWidth, int p_areaHeight, CellPredicate p_pfnPred, void *p_pParam);

	protected:
		void FromWorldToGrid(const Vector2 &p_worldPosition, Vector2 &p_gridPosition);
		void FromGridToWorld(const Vector2 &p_gridPosition, Vector2 &p_worldPosition);
		void ForEachObj(RegObjCallback p_pfnCallback);
		void ClearMap();
		void ResetStats();
		bool InBound(int p_gridX, int p_gridY);

		RegObjectList	m_registeredObjects;
		int				m_cellSide;
		int				m_numCells;
		int				m_worldWidth;
		int				m_worldHeight;
		int				m_gridWidth;
		int				m_gridHeight;
		IMStatistics	m_statistics;
		TCell*			m_pMap;
		IMType			m_typeId;
	};
}

#endif // INFLUENCEMAP_H
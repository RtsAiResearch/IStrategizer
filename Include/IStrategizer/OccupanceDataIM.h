#ifndef OCCUPANCEDATAIM_H
#define OCCUPANCEDATAIM_H

#include "InfluenceMap.h"

namespace IStrategizer
{
	using namespace MetaData;
	
	class OccupanceDataIM : public InfluenceMap
	{
	public:
		OccupanceDataIM(IMType p_typeId) : InfluenceMap(p_typeId) {}
		void Update(unsigned p_gameCycle);
		void UnregisterGameObj(TID p_objectId);
		bool IsAreaOccupied(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight);
		bool ReserveArea(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight);

	private:
		static bool OccupancePredicate(unsigned p_cellX, unsigned p_cellY, TCell* p_pCell, void *p_pParam);
		static bool ReservePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_pCell, void *p_pParam);
	};

}

#endif // OCCUPANCEDATAIM_H
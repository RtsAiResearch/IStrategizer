#ifndef IMSYSTEMMANAGER_H
#define IMSYSTEMMANAGER_H

#include "EngineData.h"
#include <vector>

namespace IStrategizer
{
	using namespace MetaData;
	class InfluenceMap;

	enum IMType
	{
		IM_BuildingData,
		IM_GroundControl
	};

	typedef map<IMType, InfluenceMap*> IMContainer;

	struct IMSysManagerParam
	{
		int BuildingDataIMCellSize;
		int GroundControlIMCellSize;
	};

	class IMSystemManager
	{
	public:
		void Update(unsigned p_gameCycle);
		void RegisterGameObj(TID p_objId, PlayerType p_ownerId);
		void UnregisterGameObj(TID p_objId);
		void RegisterIM(InfluenceMap *p_pMap, IMType p_mapTypeId);
		void Init(const IMSysManagerParam& p_param);
		void Finalize();
		InfluenceMap* GetIM(IMType p_mapTypeId);
		static IMSystemManager& Instance() { static IMSystemManager inst; return inst; }

	private:
		IMSystemManager() {}
		bool		m_initialized;
		IMContainer m_managedMaps;
	};

#define g_IMSysMgr IMSystemManager::Instance()
}


#endif // IMSYSTEMMANAGER_H
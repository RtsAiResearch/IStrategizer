#ifndef IMSYSTEMMANAGER_H
#define IMSYSTEMMANAGER_H

#include <vector>
#include "EngineData.h"
#include "WorldClock.h"
#include "RtsGame.h"

namespace IStrategizer
{
    class InfluenceMap;

    enum IMType
    {
        IM_BuildingData,
        IM_GroundControl
    };

    typedef map<IMType, InfluenceMap*> IMContainer;

    struct IMSysManagerParam
    {
        unsigned GrndCtrlIMUpdateInterval;
        unsigned OccupanceIMUpdateInterval;
        int BuildingDataIMCellSize;
        int GroundControlIMCellSize;
    };

    class IMSystemManager
    {
    public:
        void Update(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void RegisterGameObj(RtsGame& p_RtsGame, TID p_objId, PlayerType p_ownerId);
        void UnregisterGameObj(TID p_objId);
        void RegisterIM(InfluenceMap *p_pMap, IMType p_mapTypeId);
        void Init(RtsGame& p_RtsGame, const IMSysManagerParam& p_param);
        void Finalize();
        InfluenceMap* GetIM(IMType p_mapTypeId);
        static IMSystemManager& Instance() { static IMSystemManager inst; return inst; }

    private:
        IMSystemManager()
        : m_initialized(false),
        m_lastUpdateTimeMs(0)
        {}

        bool m_initialized;
        IMContainer m_managedMaps;
        IMSysManagerParam m_params;
        unsigned m_lastUpdateTimeMs;
    };

#define g_IMSysMgr IMSystemManager::Instance()
}

#endif // IMSYSTEMMANAGER_H

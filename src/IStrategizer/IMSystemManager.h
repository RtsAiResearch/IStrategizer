#ifndef IMSYSTEMMANAGER_H
#define IMSYSTEMMANAGER_H

#include "EngineData.h"
#include "WorldClock.h"
#include <vector>

namespace IStrategizer
{
    class InfluenceMap;

    enum IMType
    {
        IM_BuildingData,
        IM_GroundControl
    };

    typedef std::map<IMType, InfluenceMap*> IMContainer;

    struct IMSysManagerParam
    {
        unsigned GrndCtrlIMUpdateInterval;
        unsigned OccupanceIMUpdateInterval;
        int OccupanceIMCellSize;
        int GrndCtrllIMCellSize;
    };

    class IMSystemManager
    {
    public:
        void Update(const WorldClock& p_clock);
        void RegisterGameObj(TID p_objId, PlayerType p_ownerId);
        void UnregisterGameObj(TID p_objId);
        void RegisterIM(InfluenceMap *p_pMap, IMType p_mapTypeId);
        void Init(const IMSysManagerParam& p_param);
        void Finalize();
        InfluenceMap* GetIM(IMType p_mapTypeId);
        void DebugDumpIMs();
        static IMSystemManager& Instance() { static IMSystemManager inst; return inst; }

    private:
        IMSystemManager()
        : m_initialized(false)
        {}

        bool m_initialized;
        IMContainer m_managedMaps;
        IMSysManagerParam m_params;
    };

#define g_IMSysMgr IMSystemManager::Instance()
}

#endif // IMSYSTEMMANAGER_H

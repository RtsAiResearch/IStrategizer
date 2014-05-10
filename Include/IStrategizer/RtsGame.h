///> [Serializable]
#ifndef RTSGAME_H
#define RTSGAME_H

#ifndef DYNAMICMAP_H
#include "DynamicMap.h"
#endif
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif

#include <vector>
#include <map>

namespace IStrategizer
{
    template<class TKey, class TValue>
    class MapEx;
    enum PlayerType;
    enum EntityClassType;
    enum ResearchType;
    class GamePlayer;
    class GameType;
    class GameEntity;
    class GameResearch;
    class WorldMap;

    ///> class=RtsGame
    class RtsGame : public EngineComponent
    {
    public:
        RtsGame() : EngineComponent("game"), m_pMap(nullptr), m_initialized(false) {}
        virtual ~RtsGame();
        virtual void Init();
        virtual void Finalize();
        virtual void DisplayMessage(const char* p_msg) = 0;
        virtual size_t GetMaxTrainingQueueCount() const = 0;
        virtual EntityClassType GetResourceSource(ResourceType p_type) const = 0;
        virtual int BaseSupplyAmount() const = 0;
        virtual int SupplyBuildingSupplyAmount() const = 0;
        void Players(std::vector<PlayerType>& p_playerIds);
        void EntityTypes(std::vector<EntityClassType>& p_entityTypeIds);
        void Researches(std::vector<ResearchType>& p_researchTypeIds);
        int GetForceSizeCount( ForceSizeType p_forceSizeType );
        ForceSizeType GetForceSizeType(int forceCount);
        GamePlayer* GetPlayer(PlayerType p_id);
        GameType* GetEntityType(EntityClassType p_id);
        GameResearch* GetResearch(ResearchType p_id);
        GamePlayer* Self();
        GamePlayer* Enemy();
        WorldMap* Map();
        float GetResourceConsumbtionRatePerWorker(ResourceType p_id);

    protected:
        void InitializeTypes();
        void InitializeResearches();
        void InitializePlayers();
        virtual void InitializeMap() = 0;
        virtual GameType* FetchEntityType(EntityClassType p_id) = 0;
        virtual GameResearch* FetchResearch(ResearchType p_id) = 0;
        virtual GamePlayer* FetchPlayer(PlayerType p_id) = 0;
        virtual void EnumeratePlayers() = 0;
        virtual void EnumerateEntityTypes() = 0;
        virtual void EnumerateResearches() = 0;
        virtual int GetMaxForceSize() const = 0;

        // Game types are shared across all RtsGame instances
        static MapEx<EntityClassType, GameType*> sm_entityTypes;
        static MapEx<ResearchType, GameResearch*> sm_researches;
        static bool sm_gameTypesInitialized;
        // Been determined empirically by analyzing professional games. 
        // Refer to paper "Build Order Optimization in StarCraft" page 3
        static const float MineralsPerWorkerPerFrame; 
        static const float GasPerWorkerPerFrame;


        MapEx<PlayerType, GamePlayer*> m_players;
        WorldMap* m_pMap;
        bool m_initialized;
    };
}

extern IStrategizer::RtsGame* g_Game;

#endif // RTSGAME_H

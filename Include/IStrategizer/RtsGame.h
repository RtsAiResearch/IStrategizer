///> [Serializable]
#ifndef RTSGAME_H
#define RTSGAME_H

#include "EngineData.h"
#include "UserObject.h"
#include "SMap.h"

#include <vector>

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
    class RtsGame : public Serialization::UserObject
    {
        OBJECT_MEMBERS(1, &m_players);

    public:
        RtsGame() :
            m_pMap(nullptr),
            m_initialized(false),
            m_isOnline(true)
        {}

        virtual ~RtsGame();
        virtual void Init();
        virtual void DisplayMessage(const char* p_msg) = 0;
        virtual size_t GetMaxTrainingQueueCount() const = 0;
        virtual EntityClassType GetResourceSource(ResourceType p_type) const = 0;
        virtual int BaseSupplyAmount() const = 0;
        virtual int SupplyBuildingSupplyAmount() const = 0;
        virtual float GetResourceConsumbtionRatePerWorker(ResourceType p_id) = 0;

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
        bool IsOnline() const { return m_isOnline; }
        RtsGame* Clone() const;

    protected:
        void InitTypes();
        void InitResearches();
        void InitPlayers();
        virtual void InitMap() = 0;
        virtual GameType* FetchEntityType(EntityClassType p_id) = 0;
        virtual GameResearch* FetchResearch(ResearchType p_id) = 0;
        virtual GamePlayer* FetchPlayer(PlayerType p_id) = 0;
        virtual void EnumeratePlayers() = 0;
        virtual void EnumerateEntityTypes() = 0;
        virtual void EnumerateResearches() = 0;
        virtual int GetMaxForceSize() const = 0;
        virtual void Finalize();


        // Game types are shared across all RtsGame instances
        static Serialization::SMap<EntityClassType, GameType*> sm_entityTypes;
        static Serialization::SMap<ResearchType, GameResearch*> sm_researches;
        static bool sm_gameTypesInitialized;

        ///> type=map(pair(int,GamePlayer*))
        Serialization::SMap<PlayerType, GamePlayer*> m_players;
        WorldMap* m_pMap;
        bool m_initialized;
        bool m_isOnline;
    };
}

extern IStrategizer::RtsGame* g_Game;

#endif // RTSGAME_H

#ifndef RTSGAME_H
#define RTSGAME_H

#include <vector>
#include <map>
#include "DynamicMap.h"
#include "EngineData.h"
#include "EngineComponent.h"

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
    class EngineComponent;

    class RtsGame : public EngineComponent
    {
    protected:
        MapEx<PlayerType, GamePlayer*> m_players;
        MapEx<EntityClassType, GameType*> m_entityTypes;
        MapEx<ResearchType, GameResearch*> m_researches;
        WorldMap* m_pMap;
        bool m_initialized;

        virtual GamePlayer* FetchPlayer(RtsGame& p_RtsGame, PlayerType p_id) = 0;
        virtual GameType* FetchEntityType(EntityClassType p_id) = 0;
        virtual GameResearch* FetchResearch(ResearchType p_id) = 0;
        virtual void EnumeratePlayers() = 0;
        virtual void EnumerateEntityTypes() = 0;
        virtual void EnumerateResearches() = 0;
        void InitializeResearches();
        void InitializeTypes();
        void InitializePlayers();
        virtual void InitializeMap() = 0;

    public:
        RtsGame() : EngineComponent(*this, "game"), m_pMap(nullptr), m_initialized(false) { }
        virtual ~RtsGame();
        virtual void Init();
        virtual void Finalize();
        void Players(std::vector<PlayerType>& p_playerIds);
        void EntityTypes(std::vector<EntityClassType>& p_entityTypeIds);
        void Researches(std::vector<ResearchType>& p_researchTypeIds);
        GamePlayer* GetPlayer(PlayerType p_id);
        GameType* GetEntityType(EntityClassType p_id);
        GameResearch* GetResearch(ResearchType p_id);
        GamePlayer* Self();
        GamePlayer* Enemy();
        virtual void DisplayMessage(const char* p_msg) = 0;
        WorldMap* Map();
    };
}

#endif // RTSGAME_H

#ifndef RTSGAME_H
#define RTSGAME_H

#include <vector>
#include <map>

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif
#include "IClonable.h"
#include "GameType.h"
#include "GameResearch.h"
#include "MapEx.h"

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

    class RtsGame : public EngineComponent, public IClonable
    {
    public:
        typedef MapEx<PlayerType, GamePlayer*> PlayersMap;
        typedef MapEx<EntityClassType, GameTypeStrongPtr> TypesMap;
        typedef MapEx<ResearchType, GameResearchStrongPtr> ResearchesMap;

    protected:
        PlayersMap m_players;
        WorldMap* m_pMap;
        TypesMap m_entityTypes;
        ResearchesMap m_researches;
        bool m_initialized;

        virtual GamePlayer*     FetchPlayer(PlayerType id) = 0;
        virtual GameType*       FetchEntityType(EntityClassType id) = 0;
        virtual GameResearch*   FetchResearch(ResearchType id) = 0;
        virtual void            EnumeratePlayers() = 0;
        virtual void            EnumerateEntityTypes() = 0;
        virtual void            EnumerateResearches() = 0;
        void                    InitializeResearches();
        void                    InitializeTypes();
        void                    InitializePlayers();
        virtual void            InitializeMap() = 0;
        virtual int             GetMaxForceSize() = 0;

    public:
                        RtsGame() : EngineComponent("game"), m_pMap(nullptr), m_initialized(false) {}
        virtual         ~RtsGame();
        virtual void    Init();
        virtual void    Finalize();
        void            Players(std::vector<PlayerType>& playerIds);
        void            EntityTypes(std::vector<EntityClassType>& entityTypeIds);
        void            Researches(std::vector<ResearchType>& researchTypeIds);
        int             GetForceSizeCount( ForceSizeType forceSizeType );
        GamePlayer*     GetPlayer(PlayerType id);
        GameTypeStrongPtr     GetEntityType(EntityClassType id);
        GameResearchStrongPtr GetResearch(ResearchType id);
        GamePlayer*     Self();
        GamePlayer*     Enemy();
        virtual void    DisplayMessage(const char* pMsg) = 0;
        WorldMap*       Map();
        void            Copy(IClonable* pDest);
    };
}

extern IStrategizer::RtsGame* g_Game;

#endif // RTSGAME_H

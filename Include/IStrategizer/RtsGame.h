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
using namespace std;

namespace DataStructure
{
    template<class TKey, class TValue>
    class MapEx;
}

namespace MetaData
{
    enum PlayerType;
    enum EntityClassType;
    enum ResearchType;
}

namespace IStrategizer
{
    class GamePlayer;
    class GameType;
    class GameEntity;
    class GameResearch;
    class WorldMap;
    using namespace MetaData;

	class RtsGame : public EngineComponent
    {
    protected:
        MapEx<PlayerType, GamePlayer*>      m_players;
        MapEx<EntityClassType, GameType*>   m_entityTypes;
        MapEx<ResearchType, GameResearch*>  m_researches;
        WorldMap*                           m_pMap;
        bool                                m_initialized;

        virtual GamePlayer*     FetchPlayer(PlayerType p_id) = 0;
        virtual GameType*       FetchEntityType(EntityClassType p_id) = 0;
        virtual GameResearch*   FetchResearch(ResearchType p_id) = 0;
        virtual void            EnumeratePlayers() = 0;
        virtual void            EnumerateEntityTypes() = 0;
        virtual void            EnumerateResearches() = 0;
        void                    InitializeResearches();
        void                    InitializeTypes();
        void                    InitializePlayers();
        virtual void            InitializeMap() = 0;

    public:
                        RtsGame() : EngineComponent("game"), m_pMap(NULL), m_initialized(false) {}
        virtual         ~RtsGame();
        virtual void    Init();
		virtual	void	Finalize();
        void            Players(vector<PlayerType>& p_playerIds);
        void            EntityTypes(vector<EntityClassType>& p_entityTypeIds);
        void            Researches(vector<ResearchType>& p_researchTypeIds);
        GamePlayer*     GetPlayer(PlayerType p_id);
        GameType*       GetEntityType(EntityClassType p_id);
        GameResearch*   GetResearch(ResearchType p_id);
		GamePlayer*		Self();
		GamePlayer*		Enemy();
        virtual void    DisplayMessage(const char* p_msg) = 0;
        WorldMap*       Map();
    };
}

extern IStrategizer::RtsGame* g_Game;

#endif // RTSGAME_H
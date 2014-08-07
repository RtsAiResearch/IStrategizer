///> [Serializable]
#ifndef RTSGAME_H
#define RTSGAME_H

#include "EngineData.h"
#include "EngineObject.h"
#include "SMap.h"
#include "WorldClock.h"
#include "Vector2.h"
#include <vector>
#include "RtsAiEngine.h"

namespace IStrategizer
{
    enum PlayerType;
    enum EntityClassType;
    enum ResearchType;
    class GamePlayer;
    class GameType;
    class GameEntity;
    class GameResearch;
    class GameRace;
    class WorldMap;
    class SimilarityWeightModel;
    class IRtsGame;

    typedef Serialization::SMap<EntityClassType, GameType*> EntiyTypesMap;
    typedef Serialization::SMap<ResearchType, GameResearch*> ResearchTypesMap;
    typedef Serialization::SMap<TID, GameRace*> RaceTypesMap;

#define GAME_STATIC_DATA_FILENAME "GameStaticData.bin"

    ///> class=RtsGameStaticData
    class RtsGameStaticData : public EngineObject
    {
        OBJECT_SERIALIZABLE(RtsGameStaticData, &EntityTypes, &ResearchTypes);

    public:
        ~RtsGameStaticData();

        ///> type=map(pair(int,GameType*)
        EntiyTypesMap EntityTypes;
        ///> type=map(pair(int,GameResearch*)
        ResearchTypesMap ResearchTypes;
        RaceTypesMap RaceTypes;
    };


    ///> class=RtsGame
    class RtsGame : public EngineObject
    {
        OBJECT_SERIALIZABLE(RtsGame, &m_isOnline, &m_players, &m_cachedGameFrame, &m_cachedWorldWidth, &m_cachedWorldHeight);

    public:
        RtsGame() :
            m_pMap(nullptr),
            m_isInitialized(false),
            m_isOnline(true),
            m_cachedGameFrame(0),
            m_firstUpdate(true)
        {}

        ~RtsGame();
        bool Init();
        static void FinalizeStaticData();
        void ExportStaticData();

        const EntiyTypesMap& EntityTypes() const { return sm_pGameStatics->EntityTypes; }
        const ResearchTypesMap& ResearchTypes() const { return sm_pGameStatics->ResearchTypes; }
        const RaceTypesMap& RaceTypes() const { return sm_pGameStatics->RaceTypes; }
        const Serialization::SMap<PlayerType, GamePlayer*>& Players() const { return m_players; }

        GamePlayer* GetPlayer(PlayerType p_id) const;
        GameType* GetEntityType(EntityClassType p_id);
        GameResearch* GetResearch(ResearchType p_id);
        GameRace* GetRace(TID raceID);
        GameEntity* GetEntity(_In_ TID entityId);

        GamePlayer* Self() { return GetPlayer(PLAYER_Self); }
        const GamePlayer* Self() const { return GetPlayer(PLAYER_Self); }
        GamePlayer* Enemy() { return GetPlayer(PLAYER_Enemy); }

        WorldMap* Map() { _ASSERTE(m_isInitialized); return m_pMap; }
        const WorldMap* Map() const { _ASSERTE(m_isInitialized); return m_pMap; }

        RtsGame* Snapshot() const;
        virtual int GameFrame() const;
        float Distance(const RtsGame* pOther, const SimilarityWeightModel* pModel) const;
        const WorldClock& Clock() const { return m_clock; }
        void Update();

        // Debugging Draw Helpers
        virtual void DebugDrawMapLine(_In_ Vector2 p1, _In_ Vector2 p2, _In_ GameDrawColor c);
        virtual void DebugDrawMapCircle(_In_ Vector2 p, _In_ int r, _In_ GameDrawColor c, _In_ bool fill = false);
        virtual void DebugDrawMapText(_In_ Vector2 p, _In_ const char* pTxt);
        virtual void DebugDrawMapRectangle(_In_ Vector2 topLeft, _In_ Vector2 bottomRight, _In_ GameDrawColor c, _In_ bool fill = false);
        virtual void DebugDrawScreenText(_In_ Vector2 p, _In_ const char* pTxt, _In_ GameDrawColor c);
        virtual void DebugDraw();

        static SimilarityWeightModel DefaultWeightModel;
        // Game types are shared across all RtsGame instances
        static RtsGameStaticData* sm_pGameStatics;

    protected:
        void SetOffline();
        virtual void InitPlayers();
        virtual void InitMap();
        virtual void Finalize();
        void InitEntityTypes();
        void InitResearchTypes();
        void InitRaceTypes();
        bool InitStaticData();

        ///> type=bool
        bool m_isOnline;
        ///> type=map(pair(int,GamePlayer*))
        Serialization::SMap<PlayerType, GamePlayer*> m_players;
        ///> type=unsigned
        unsigned m_cachedGameFrame;
        ///> type=unsigned
        unsigned m_cachedWorldWidth;
        ///> type=unsigned
        unsigned m_cachedWorldHeight;

        bool m_isInitialized;
        WorldMap* m_pMap;
        WorldClock m_clock;
        bool m_firstUpdate;
    };

    struct RtsGameModelAttributeWeights
    {
        float W[COUNT(RtsGameModelAttributeWeight)];
    };

    class RtsGameModel
    {
    public:
        /*
        Current Game Model Features:
        GameFrame
        MapArea
        Player.Entities.NumAttackingUnits
        Player.Entities.NumBuildings
        Player.Entities.NumWorkers
        Player.Entities.NumDoneResearches
        Player.Resources.Primary
        Player.Resources.Secondary
        Player.Resources.Supply
        */
        RtsGameModel(_In_ const RtsGame& game) :
            m_game(game)
        {
        }

        int Attr(_In_ RtsGameModelAttribute attr) const;
        float Distance(_In_ const RtsGameModel& other, _In_ const RtsGameModelAttributeWeights& weights) const;

    private:
        RtsGameModel(const RtsGameModel&);
        void operator=(const RtsGameModel&);

        const RtsGame& m_game;
    };
}

extern IStrategizer::RtsGame* g_Game;
extern IStrategizer::IRtsGame* g_GameImpl;

#endif // RTSGAME_H

///> [Serializable]
#ifndef RTSGAME_H
#define RTSGAME_H

#include "EngineData.h"
#include "UserObject.h"
#include "SMap.h"
#include <vector>

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

    typedef Serialization::SMap<EntityClassType, GameType*> EntiyTypesMap;
    typedef Serialization::SMap<ResearchType, GameResearch*> ResearchTypesMap;
    typedef Serialization::SMap<TID, GameRace*> RaceTypesMap;

    ///> class=RtsGame
    class RtsGame : public Serialization::UserObject
    {
        OBJECT_MEMBERS(5, &m_isOnline, &m_players, &m_cachedGameFrame, &m_cachedWorldWidth, &m_cachedWorldHeight);

    public:
        RtsGame() :
            m_pMap(nullptr),
            m_isInitialized(false),
            m_isOnline(true),
            m_cachedGameFrame(0)
        {}

        virtual ~RtsGame();
        virtual void Init();
        virtual void DisplayMessage(const char* p_msg) = 0;

        void Players(std::vector<PlayerType>& p_playerIds) { m_players.Keys(p_playerIds); }
        void EntityTypes(std::vector<EntityClassType>& p_entityTypeIds) { sm_entityTypes.Keys(p_entityTypeIds); }
        void Researches(std::vector<ResearchType>& p_researchTypeIds) const { sm_researchTypes.Keys(p_researchTypeIds); }

        const EntiyTypesMap& EntityTypes() const { return sm_entityTypes; }
        const ResearchTypesMap& ResearchTypes() const { return sm_researchTypes; }
        const RaceTypesMap& RaceTypes() const { return sm_raceTypes; }

        GamePlayer* GetPlayer(PlayerType p_id) const;
        GameType* GetEntityType(EntityClassType p_id);
        GameResearch* GetResearch(ResearchType p_id);
        GameRace* GetRace(TID raceID);
        GamePlayer* Self() { return GetPlayer(PLAYER_Self); }
        const GamePlayer* Self() const { return GetPlayer(PLAYER_Self); }
        GamePlayer* Enemy() { return GetPlayer(PLAYER_Enemy); }
        WorldMap* Map() { _ASSERTE(m_isInitialized); return m_pMap; }
        const WorldMap* Map() const { _ASSERTE(m_isInitialized); return m_pMap; }
        RtsGame* Snapshot() const;
        virtual size_t GetMaxTrainingQueueCount() const = 0;
        virtual unsigned GameFrame() const = 0;
        float Distance(const RtsGame* pOther, const SimilarityWeightModel* pModel) const;

        static SimilarityWeightModel DefaultWeightModel;

    protected:
        void SetOffline();
        virtual void InitEntityTypes() = 0;
        virtual void InitResearchTypes() = 0;
        virtual void InitRaceTypes() = 0;
        virtual void InitPlayers() = 0;
        virtual void InitMap() = 0;
        virtual GameType* FetchEntityType(EntityClassType p_id) = 0;
        virtual GameResearch* FetchResearch(ResearchType p_id) = 0;
        virtual GamePlayer* FetchPlayer(PlayerType p_id) = 0;
        virtual int GetMaxForceSize() const = 0;
        virtual void Finalize();

        // Game types are shared across all RtsGame instances
        static EntiyTypesMap sm_entityTypes;
        static ResearchTypesMap sm_researchTypes;
        static RaceTypesMap sm_raceTypes;
        static bool sm_gameTypesInitialized;

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
    };

    struct RtsGameModelAttributeWeights
    {
        int W[COUNT(RtsGameModelAttributeWeight)];
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
        RtsGameModel(const RtsGameModel&);                 \
        void operator=(const RtsGameModel&);

        const RtsGame& m_game;
    };
}

extern IStrategizer::RtsGame* g_Game;

#endif // RTSGAME_H

///> [Serializable]
#ifndef STARCRAFTGAME_H
#define STARCRAFTGAME_H

#ifndef RTSGAME_H
#include "RtsGame.h"
#endif

namespace IStrategizer
{
    ///> class=StarCraftGame
    ///> parent=RtsGame
    class StarCraftGame : public RtsGame
    {
		OBJECT_SERIALIZABLE_P(StarCraftGame, RtsGame);

    public:
        void ExecuteCommand(const char *p_cmd);
        void DisplayMessage(const char *p_msg);
        int GetMaxForceSize() const { return 12; }
        size_t GetMaxTrainingQueueCount() const { return 5; }
        unsigned GameFrame() const;
        void DebugDrawMapLine(_In_ Vector2 p1, _In_ Vector2 p2, _In_ GameDrawColor c);
        void DebugDrawMapCircle(_In_ Vector2 p, _In_ int r, _In_ GameDrawColor c, _In_ bool fill = false);
        void DebugDrawMapText(_In_ Vector2 p, _In_ const std::string& txt);
        void DebugDrawMapRectangle(_In_ Vector2 topLeft, _In_ Vector2 bottomRight, _In_ GameDrawColor c, _In_ bool fill = false);
        void DebugDrawScreenText(_In_ Vector2 p, _In_ const std::string& txt, _In_ GameDrawColor c);

    protected:
        void InitPlayers();
        void InitMap();
        void InitEntityTypes();
        void InitResearchTypes();
        void InitRaceTypes();
        bool InitStaticData();

        GamePlayer* FetchPlayer(PlayerType p_id);
        IStrategizer::GameType* FetchEntityType(EntityClassType p_id);
        GameResearch* FetchResearch(ResearchType p_id);
    };
}

#endif // STARCRAFTGAME_H

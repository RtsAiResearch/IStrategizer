#ifndef STARCRAFTGAME_H
#define STARCRAFTGAME_H

#ifndef RTSGAME_H
#include "RtsGame.h"
#endif

namespace StarCraftModel
{
    using namespace IStrategizer;

    class StarCraftGame : public RtsGame
    {
    protected:
        void EnumeratePlayers();
        void EnumerateEntityTypes();
        void EnumerateResearches();
        void InitializeMap();
        int GetMaxForceSize();
        GamePlayer* FetchPlayer(IStrategizer::PlayerType p_id);
        IStrategizer::GameType* FetchEntityType(EntityClassType p_id);
        GameResearch* FetchResearch(ResearchType p_id);

    public:
        StarCraftGame();
        void ExecuteCommand(const char *p_cmd);
        void DisplayMessage(const char *p_msg);
        size_t GetMaxTrainingQueueCount();
    };
}

#endif // STARCRAFTGAME_H

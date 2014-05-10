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
        GamePlayer* FetchPlayer(IStrategizer::PlayerType p_id);
        IStrategizer::GameType* FetchEntityType(EntityClassType p_id);
        GameResearch* FetchResearch(ResearchType p_id);
        int GetMaxForceSize() const;

    public:
        StarCraftGame();
        void ExecuteCommand(const char *p_cmd);
        void DisplayMessage(const char *p_msg);
        size_t GetMaxTrainingQueueCount() const;
        int BaseSupplyAmount() const;
        int SupplyBuildingSupplyAmount() const;
        EntityClassType GetResourceSource(ResourceType p_type) const;
    };
}

#endif // STARCRAFTGAME_H

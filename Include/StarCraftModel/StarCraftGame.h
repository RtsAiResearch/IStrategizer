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
        OBJECT_SERIALIZABLE(StarCraftGame);

    public:
        StarCraftGame();
        void ExecuteCommand(const char *p_cmd);
        void DisplayMessage(const char *p_msg);
        size_t GetMaxTrainingQueueCount() const;
        int BaseSupplyAmount() const;
        int SupplyBuildingSupplyAmount() const;
        EntityClassType GetResourceSource(ResourceType p_type) const;
        float GetResourceConsumbtionRatePerWorker(ResourceType p_id);

    protected:
        void EnumeratePlayers();
        void EnumerateEntityTypes();
        void EnumerateResearches();
        void InitMap();
        GamePlayer* FetchPlayer(IStrategizer::PlayerType p_id);
        GameType* FetchEntityType(EntityClassType p_id);
        GameResearch* FetchResearch(ResearchType p_id);
        int GetMaxForceSize() const;

        static const float MineralsPerWorkerPerFrame; 
        static const float GasPerWorkerPerFrame;
    };
}

#endif // STARCRAFTGAME_H

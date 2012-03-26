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
        void            EnumeratePlayers();
		void			EnumerateEntityTypes();
		void			EnumerateResearches();
        GamePlayer*     FetchPlayer(PlayerType p_id);
        GameType*       FetchEntityType(EntityClassType p_id);
        GameResearch*   FetchResearch(ResearchType p_id);
        void            InitializeMap();
    public:
		StarCraftGame();
		void	ExecuteCommand(const char *p_cmd);
        void    DisplayMessage(const char *p_msg);

    };
}

#endif // STARCRAFTGAME_H
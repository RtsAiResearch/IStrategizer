#ifndef STARCRAFTENTITY_H
#define STARCRAFTENTITY_H

#ifndef GAMEENTITY_H
#include "GameEntity.h"
#endif

namespace BWAPI
{
	class Unit;
	class Game;
}

namespace MetaData
{
    enum ObjectStateType;
	enum EntityClassType;
	enum PlayerType;
}

namespace StarCraftModel
{
    using namespace IStrategizer;
	using namespace MetaData;
	using namespace BWAPI;

    class StarCraftEntity : public GameEntity
    {

    public:
		StarCraftEntity(Unit* p_unit);
		int	Attr(EntityObjectAttribute p_attrId);
		int	Research(ResearchType p_researchId);
		int	Build(EntityClassType p_buildingClassId, int p_x, int p_y);
		int	AttackGround(int p_x, int p_y);
		int	AttackEntity(PlayerType p_opponentIndex, int p_targetEntityObjectId);
		int	Train(EntityClassType p_entityClassId);

	protected:
		ObjectStateType FetchState();

	private:
		Unit*	m_unit;
		MetaData::PlayerType m_ownerId;
    };
}

#endif // STARCRAFTENTITY_H
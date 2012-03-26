#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif

class Action;

namespace MetaData
{
    enum EntityClassType;
    enum EntityObjectAttribute;
	enum PlayerType;
}

namespace IStrategizer
{
    using namespace MetaData;
    class GameEntity
    {
    public:
		GameEntity(TID p_id) : m_id(p_id) {}
		virtual ~GameEntity() {}
        TID             Id() const { return m_id; }
        EntityClassType Type() const { return m_type; }
        virtual int		Attr(EntityObjectAttribute p_attrId) = 0;
		virtual int		Research(ResearchType p_researchId) = 0;
		virtual int		Build(EntityClassType p_buildingClassId, int p_x, int p_y) = 0;
		virtual int		AttackGround(int p_x, int p_y) = 0;
		virtual int		AttackEntity(PlayerType p_opponentIndex, int p_targetEntityObjectId) = 0;
		virtual int		Train(EntityClassType p_entityClassId) = 0;
		bool			Locked() { return m_locked; }
		void			Lock(Action *pOwner = 0) { m_locked = true; }
		void			Unlock(Action *pOwner = 0) { m_locked = false; }

	protected:
		TID             m_id;
		EntityClassType m_type;
		bool			m_locked;
		Action			*m_pOwner;
	};
}

#endif // GAMEENTITY_H
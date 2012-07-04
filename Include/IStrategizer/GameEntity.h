#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef SHAREDRESOURCE_H
#include "SharedResource.h"
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
	class GameEntity : public SharedResource
    {
    public:
		GameEntity(TID p_id) : m_id(p_id) {}
		virtual ~GameEntity() {}
        TID             Id() const { return m_id; }
        EntityClassType Type() const { return m_type; }
        virtual int Attr(EntityObjectAttribute p_attrId) = 0;
		virtual bool Research(ResearchType p_researchId) = 0;
		virtual bool Build(EntityClassType p_buildingClassId, int p_x, int p_y) = 0;
		virtual bool AttackGround(int p_x, int p_y) = 0;
		virtual bool AttackEntity(PlayerType p_opponentIndex, int p_targetEntityObjectId) = 0;
		virtual bool Train(EntityClassType p_entityClassId) = 0;
		bool Acquire() { return true; }
		bool Release() { return true; }

	protected:
		TID             m_id;
		EntityClassType m_type;
	};
}

#endif // GAMEENTITY_H
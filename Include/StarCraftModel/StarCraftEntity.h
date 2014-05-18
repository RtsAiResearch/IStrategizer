#ifndef STARCRAFTENTITY_H
#define STARCRAFTENTITY_H

#ifndef GAMEENTITY_H
#include "GameEntity.h"
#endif

#include "BWAPI.h"

#define TilePositionFromUnitPosition(UnitPos) (UnitPos / 32)
#define UnitPositionFromTilePosition(TilePos) (TilePos * 32)

namespace IStrategizer
{
    enum ObjectStateType;
    enum EntityClassType;
    enum PlayerType;
    class RtsGame;

    class StarCraftEntity: public GameEntity
    {
        OBJECT_SERIALIZABLE(StarCraftEntity);
        OBJECT_MEMBERS_P(GameEntity, 2, &m_isOnline, &m_cachedAttr);

    public:
        StarCraftEntity() : GameEntity(INVALID_TID), m_pUnit(nullptr) {}
        StarCraftEntity(BWAPI::Unit p_unit);

        // Game Properties
        IStrategizer::Vector2 GetPosition() const;
        int Attr(EntityObjectAttribute p_attrId) const;
        bool IsTraining(TID p_traineeId) const;
        bool IsNull() { return m_isOnline && m_pUnit == nullptr; }
        bool CanGather(TID resourceObjectId) const;
        std::string ToString() const;

        // Game Commands
        bool Research(IStrategizer::ResearchType p_researchId);
        bool Build(EntityClassType p_buildingClassId, Vector2 p_position);
        bool AttackGround(Vector2 p_position);
        bool AttackEntity(TID p_targetEntityObjectId);
        bool Train(EntityClassType p_entityClassId);
        bool Move(Vector2 p_position);
		bool GatherResourceEntity(TID p_resourceEntityObjectId);

        void SetOffline(RtsGame* pBelongingGame);

    protected:
        ObjectStateType FetchState() const;
        ///> type=int
        bool m_isOnline;
        ///> type=map(pair(int, int))
        Serialization::SMap<EntityObjectAttribute, int> m_cachedAttr;
        BWAPI::Unit m_pUnit;
    };
}

#endif // STARCRAFTENTITY_H

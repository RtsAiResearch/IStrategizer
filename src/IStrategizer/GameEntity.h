///> [Serializable]
#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include "EngineData.h"
#include "SharedResource.h"
#include "Vector2.h"
#include "EngineObject.h"

class Action;

namespace IStrategizer
{
    enum EntityClassType;
    enum EntityObjectProperty;
    enum PlayerType;
    class RtsGame;
    class GameType;
    class EntityController;
    class IGameUnitType;

    ///> class=GameEntity
	class GameEntity : public SharedResource
    {
        OBJECT_SERIALIZABLE(GameEntity, &m_id, &m_cachedAttr, &m_isOnline);
    public:
        GameEntity();
        GameEntity(TID id);
        virtual ~GameEntity() {}

        // Game Properties
        TID Id() const { return m_id; }
        EntityClassType TypeId() const { return (EntityClassType)P(OP_TypeId); }
        const GameType* Type() const;
        PlayerType GetPlayer() const { return (PlayerType)P(OP_OwnerId); }
        virtual Vector2 Position() const;
        virtual TID TargetId() const;
        virtual Vector2 TargetPosition() const;
        virtual int P(EntityObjectProperty attrId) const;
        virtual bool IsTraining(TID traineeId) const;
        virtual bool CanGather(TID resourceObjectId) const;
        std::string ToString(bool minimal = false) const;

		// Euclidean distance squared between 2 units on the game map
		unsigned Distance(_In_ const GameEntity *pOther) const;
        void CacheAttributes();
        virtual bool Exists() const;
        virtual bool CanAttack(_In_ TID targetId) const;
        virtual bool CanRepair(TID entityId) const;
        virtual bool CanReach(_In_ Vector2 dest) const;
        virtual int LastCommandFrame() const;
        void SetController(_In_ EntityController* pController) { m_pController = pController; }
        virtual void DebugDraw();
        virtual bool CanBuild(_In_ EntityClassType typeId) const;

        // Game Commands
        virtual bool Research(ResearchType researchId);
        virtual bool Build(EntityClassType buildingClassId, Vector2 position);
        virtual bool AttackGround(Vector2 position);
        virtual bool Move(Vector2 position);
        virtual bool AttackEntity(TID targetEntityObjectId);
        virtual bool Train(EntityClassType entityClassId);
        virtual bool GatherResourceEntity(TID resourceEntityObjectId);
        virtual void CancelConstruction();
		virtual bool Stop();
		virtual bool CanTrain(EntityClassType type);
        virtual bool Repair(_In_ TID targetId);

        void DebugDrawTarget();
        void DebugDrawRange();
        void DebugDrawLineOfSight();
        void DebugDrawHealthBar();

        virtual void SetOffline(RtsGame* pBelongingGame);
        bool Acquire() { return true; }
        bool Release() { return true; }
        bool IsNull() { return m_id == INVALID_TID; }

    protected:
        void DebugDrawMapLastGameError();
        ObjectStateType FetchState() const;

        ///> type=int
        TID m_id;
        ///> type=map(pair(int,int))
        Serialization::SMap<EntityObjectProperty, int> m_cachedAttr;
        ///> type=int
        bool m_isOnline;
        int m_cacheFrame;
        EntityController* m_pController;
        EntityClassType m_typeId;
        PlayerType m_ownerId;
        const IGameUnitType* m_pUnitType;
        GameType* m_pType;
    };
}

#endif // GAMEENTITY_H

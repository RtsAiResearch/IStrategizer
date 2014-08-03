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

    ///> class=GameEntity
	class GameEntity : public SharedResource
    {
        OBJECT_SERIALIZABLE(GameEntity, &m_id, &m_cachedAttr);
    public:
        GameEntity(TID id) :
            m_id(id),
            m_cacheFrame(-1)
        {}
        virtual ~GameEntity() {}

        // Game Properties
        TID Id() const { return m_id; }
        EntityClassType TypeId() const { return (EntityClassType)P(OP_TypeId); }
        const GameType* Type() const;
        PlayerType GetPlayer() const { return (PlayerType)P(OP_OwnerId); }
        virtual Vector2 Position() const = 0;
        virtual TID TargetId() const = 0;
        virtual Vector2 TargetPosition() const = 0;
        virtual int P(EntityObjectProperty attrId) const = 0;
        virtual bool IsTraining(TID traineeId) const = 0;
        virtual bool CanGather(TID resourceObjectId) const = 0;
		// Euclidean distance squared between 2 units on the game map
		unsigned Distance(_In_ const GameEntity *pOther) const;
        void CacheAttributes();
        virtual bool Exists() const = 0;
        virtual bool CanAttack(_In_ TID targetId) const = 0;
        virtual bool CanRepair(TID entityId) const = 0;
        virtual bool CanReach(_In_ Vector2 dest) const = 0;
        virtual int LastCommandFrame() const = 0;

        // Game Commands
        virtual bool Research(ResearchType researchId) = 0;
        virtual bool Build(EntityClassType buildingClassId, Vector2 position) = 0;
        virtual bool AttackGround(Vector2 position) = 0;
        virtual bool Move(Vector2 position) = 0;
        virtual bool AttackEntity(TID targetEntityObjectId) = 0;
        virtual bool Train(EntityClassType entityClassId) = 0;
        virtual bool GatherResourceEntity(TID resourceEntityObjectId) = 0;
		virtual void CancelOrders() = 0;
		virtual bool Stop() = 0;
		virtual bool CanTrain(EntityClassType type) = 0;
        virtual bool Follow(TID entityId) = 0;
        virtual bool Repair(_In_ TID targetId) = 0;

        void DebugDrawTarget();
        void DebugDrawRange();
        void DebugDrawLineOfSight();
        void DebugDrawHealthBar();

        virtual void SetOffline(RtsGame* pBelongingGame) = 0;
        bool Acquire() { return true; }
        bool Release() { return true; }

    protected:
        ///> type=int
        TID m_id;
        ///> type=map(pair(int,int))
        Serialization::SMap<EntityObjectProperty, int> m_cachedAttr;
        int m_cacheFrame;
    };
}

#endif // GAMEENTITY_H

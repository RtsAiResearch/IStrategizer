#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include "EngineData.h"
#include "SharedResource.h"
#include "Vector2.h"
#include "UserObject.h"

class Action;

namespace IStrategizer
{
    enum EntityClassType;
    enum EntityObjectAttribute;
    enum PlayerType;
    class RtsGame;

    class GameEntity : public Serialization::UserObject, public SharedResource
    {
    public:
        GameEntity(TID id) :
            m_id(id)
        {}
        virtual ~GameEntity() {}

        // Game Properties
        TID Id() const { return m_id; }
        EntityClassType Type() const { return (EntityClassType)Attr(EOATTR_Type); }
        PlayerType GetPlayer() const { return (PlayerType)Attr(EOATTR_OwnerId); }
        virtual Vector2 GetPosition() const = 0;
        virtual int Attr(EntityObjectAttribute attrId) const = 0;
        virtual bool IsTraining(TID traineeId) const = 0;
        virtual bool CanGather(TID resourceObjectId) const = 0;
        virtual std::string ToString() const = 0;

        // Game Commands
        virtual bool Research(ResearchType researchId) = 0;
        virtual bool Build(EntityClassType buildingClassId, Vector2 position) = 0;
        virtual bool AttackGround(Vector2 position) = 0;
        virtual bool Move(Vector2 position) = 0;
        virtual bool AttackEntity(TID targetEntityObjectId) = 0;
        virtual bool Train(EntityClassType entityClassId) = 0;
        virtual bool GatherResourceEntity(TID resourceEntityObjectId) = 0;

        bool Acquire() { return true; }
        bool Release() { return true; }
        void SetOffline(RtsGame* pBelongingGame) {}

    protected:
        TID m_id;
    };
}

#endif // GAMEENTITY_H

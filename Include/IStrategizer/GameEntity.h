#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef SHAREDRESOURCE_H
#include "SharedResource.h"
#endif

#ifndef VECTOR2_H
#include "Vector2.h"
#endif
#include "IClonable.h"

namespace IStrategizer
{
    enum EntityClassType;
    enum EntityObjectAttribute;
    enum PlayerType;

    class GameEntity : public SharedResource, public IClonable
    {
    public:
        GameEntity(TID p_id) : m_id(p_id) {}
        virtual ~GameEntity() {}
        TID             Id() const { return m_id; }
        EntityClassType Type() const { return m_type; }
        bool Acquire() { return true; }
        bool Release() { return true; }
        PlayerType GetPlayer() const { return m_ownerId; }
        void Copy(IClonable* pDest);

        virtual std::string ToString() const = 0;
        virtual Vector2 GetPosition() const = 0;
        virtual bool IsTraining(TID p_traineeId) const = 0;
        virtual int Attr(EntityObjectAttribute attrId);
        virtual bool Research(ResearchType p_researchId) = 0;
        virtual bool Build(EntityClassType p_buildingClassId, Vector2 p_position) = 0;
        virtual bool AttackGround(Vector2 p_position) = 0;
        virtual bool Move(Vector2 p_position) = 0;
        virtual bool AttackEntity(TID p_targetEntityObjectId) = 0;
        virtual bool Train(EntityClassType p_entityClassId) = 0;

    protected:
        GameEntity() {}
        // Retrieves the attribute from the current game instance, not a cached value
        virtual int FetchAttr(EntityObjectAttribute attrId) const = 0;

        TID m_id;
        EntityClassType m_type;
        IStrategizer::PlayerType m_ownerId;
    };
}

#endif // GAMEENTITY_H
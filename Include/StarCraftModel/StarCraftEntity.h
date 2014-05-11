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
}

namespace StarCraftModel
{
    class StarCraftEntity: public IStrategizer::GameEntity
    {
    public:
        StarCraftEntity(BWAPI::Unit p_unit);
        std::string ToString() const;
        IStrategizer::Vector2 GetPosition() const;
        int Attr(IStrategizer::EntityObjectAttribute p_attrId) const;
        bool IsTraining(IStrategizer::TID p_traineeId) const;
        bool IsNull();
        bool IsGatheringResource(IStrategizer::ResourceType resourceType) const;
        bool CanTrain(IStrategizer::EntityClassType p_entityClassId) const;
        bool Research(IStrategizer::ResearchType p_researchId);
        bool Build(IStrategizer::EntityClassType p_buildingClassId, IStrategizer::Vector2 p_position);
        bool AttackGround(IStrategizer::Vector2 p_position);
        bool AttackEntity(IStrategizer::TID p_targetEntityObjectId);
        bool Train(IStrategizer::EntityClassType p_entityClassId);
        bool Move(IStrategizer::Vector2 p_position);
		bool GatherResourceEntity(IStrategizer::TID p_resourceEntityObjectId);

    protected:
        IStrategizer::ObjectStateType FetchState() const;

    private:
        BWAPI::Unit m_unit;
    };
}

#endif // STARCRAFTENTITY_H

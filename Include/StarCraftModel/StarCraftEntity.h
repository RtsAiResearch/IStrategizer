#ifndef STARCRAFTENTITY_H
#define STARCRAFTENTITY_H

#ifndef GAMEENTITY_H
#include "GameEntity.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
{
  enum ObjectStateType;
  enum EntityClassType;
  enum PlayerType;
}

namespace StarCraftModel
{
  using namespace IStrategizer;
  using namespace IStrategizer;
  using namespace BWAPI;

  class StarCraftEntity : public GameEntity
  {

  public:
    StarCraftEntity(Unit p_unit);
    int	Attr(EntityObjectAttribute p_attrId) const;
    std::string ToString() const;

    bool IsTraining(TID p_traineeId) const;
    bool Research(ResearchType p_researchId);
    bool Build(EntityClassType p_buildingClassId, int p_x, int p_y);
    bool AttackGround(int p_x, int p_y);
    bool AttackEntity(IStrategizer::PlayerType p_opponentIndex, int p_targetEntityObjectId);
    bool Train(EntityClassType p_entityClassId);
	std::string ToString() const;	IStrategizer::Vector2 GetPosition();
  protected:
    ObjectStateType FetchState() const;

  private:
    Unit	m_unit;
    IStrategizer::PlayerType m_ownerId;
  };
}

#endif // STARCRAFTENTITY_H
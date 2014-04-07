#include "GameEntity.h"

using namespace IStrategizer;

int GameEntity::Attr(EntityObjectAttribute attrId)
{
    return FetchAttr(attrId);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::Copy(IClonable* pDest)
{
    GameEntity *pConDest = dynamic_cast<GameEntity*>(pDest);
    _ASSERTE(pConDest);

    pConDest->m_id = m_id;
    pConDest->m_ownerId = m_ownerId;
    pConDest->m_type = m_type;
}

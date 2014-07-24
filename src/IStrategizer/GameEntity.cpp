#include "GameEntity.h"
#include "RtsGame.h"

using namespace IStrategizer;

unsigned GameEntity::Distance(_In_ const GameEntity *pOther) const
{
    int dX = Attr(EOATTR_PosCenterX) - pOther->Attr(EOATTR_PosCenterX);
    int dY = Attr(EOATTR_PosCenterY) - pOther->Attr(EOATTR_PosCenterY);
    return (unsigned)sqrt((float)(unsigned)((dX * dX) + (dY * dY)));
}
//////////////////////////////////////////////////////////////////////////
const GameType* GameEntity::Type() const
{
    return g_Game->GetEntityType(TypeId());
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::CacheAttributes()
{
    if ((int)g_Game->GameFrame() > m_cacheFrame)
    {
        LogDebugInfo("Caching %s attributes", ToString().c_str());

        m_cachedAttr.clear();
        for (int attr = START(EntityObjectAttribute);
            attr != END(EntityObjectAttribute); ++attr)
        {
            m_cachedAttr[EntityObjectAttribute(attr)] = Attr(EntityObjectAttribute(attr));
        }

        m_cacheFrame = g_Game->GameFrame();
    }
}
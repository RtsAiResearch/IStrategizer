#include "GameEntity.h"
#include "RtsGame.h"
#include "GameType.h"

using namespace IStrategizer;

unsigned GameEntity::Distance(_In_ const GameEntity *pOther) const
{
    int dX = P(OP_PosCenterX) - pOther->P(OP_PosCenterX);
    int dY = P(OP_PosCenterY) - pOther->P(OP_PosCenterY);
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

        m_cacheFrame = g_Game->GameFrame();

        m_cachedAttr.clear();
        for (int attr = START(EntityObjectProperty);
            attr != END(EntityObjectProperty); ++attr)
        {
            m_cachedAttr[EntityObjectProperty(attr)] = P(EntityObjectProperty(attr));
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawTarget()
{
    g_Game->DebugDrawMapLine(GetPosition(), GetTargetPosition(), GCLR_Red);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawRange()
{
    g_Game->DebugDrawMapCircle(GetPosition(), Type()->P(TP_GroundRange), GCLR_Yellow);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawLineOfSight()
{
    g_Game->DebugDrawMapCircle(GetPosition(), Type()->P(TP_LineOfSight), GCLR_White);
}